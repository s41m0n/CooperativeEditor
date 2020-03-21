#include <QHostAddress>
#include <spdlog/spdlog.h>

#include "Controller.h"
#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/RequestMessage.h"

Controller::Controller(Model *model, unsigned short port, QWidget *parent)
    : QTcpServer(parent), model(model) {
  if (!listen(QHostAddress::Any, port)) {
    spdlog::error("Unable to listen on address {} and port {}",
                  QHostAddress::Any, port);
    exit(-1);
  }
  connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void Controller::incomingConnection(qintptr handle) {
  auto socket = new TcpSocket();
  socket->setSocketDescriptor(handle);
  addPendingConnection(socket);
}

void Controller::onNewConnection() {

  auto clientSocket = dynamic_cast<TcpSocket *>(nextPendingConnection());
  auto clientId = clientSocket->socketDescriptor();
  clientSocket->setIdentifier(clientId);

  connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
          this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

  spdlog::debug("Connected Editor {0:d}", clientId);

  BasicMessage msg(clientId);
  clientSocket->sendMsg(Type::U_CONNECT, msg);
}

void Controller::onSocketStateChanged(
    QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
    dispatch(sender, Type::U_DISCONNECTED, Header(), std::make_shared<BasicMessage>(sender->getIdentifier()));
    model->removeUserActivity(sender);
    model->removeConnection(sender);
    sender->deleteLater();
  }
}

void Controller::onReadyRead() {
  auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
  auto clientId = sender->getIdentifier();

  if (sender->isMessageAvailable()) {

    auto header = sender->getHeader();
    std::shared_ptr<BasicMessage> base(sender->readMsg());

    switch (header.getType()) {

    case Type::U_REGISTER:
    case Type::U_LOGIN: {
      auto derived = std::dynamic_pointer_cast<UserMessage>(base);
      auto user = derived->getUser();
      bool result = header.getType() == Type::U_LOGIN ? Model::logInUser(user)
                                                      : Model::registerUser(user);
      if (result) {
        UserMessage newMsg(clientId, user);
        FileListingMessage newMsg2(clientId, model->getAvailableFiles());
        sender->sendMsg(header.getType() == Type::U_LOGIN ? Type::U_LOGIN_OK
                                                          : Type::U_REGISTER_OK,
                        newMsg);
        model->insertUserActivity(sender, user);
        sender->sendMsg(Type::F_LISTING, newMsg2);
      } else {
        BasicMessage msg(clientId);
        sender->sendMsg(header.getType() == Type::U_LOGIN ? Type::U_LOGIN_KO
                                                          : Type::U_REGISTER_KO,
                        msg);
      }
      break;
    }
    case Type::S_INSERT:
    case Type::S_ERASE: {
      auto derived = std::dynamic_pointer_cast<CrdtMessage>(base);
      try {
        header.getType() == Type::S_INSERT
            ? model->userInsert(sender, derived->getSymbol())
            : model->userErase(sender, derived->getSymbol());
        dispatch(sender, header.getType(), header, derived);
      } catch (std::exception &e) {
        spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
      }
      break;
    }
    case Type::U_UPDATE: {
      auto derived = std::dynamic_pointer_cast<UserMessage>(base);
      auto user = derived->getUser();
      if (Model::updateUser(user)) {
        UserMessage newMsg(clientId, user);
        sender->sendMsg(Type::U_UPDATE_OK, newMsg);
      } else {
        BasicMessage msg(clientId);
        sender->sendMsg(Type::U_UPDATE_KO, msg);
      }
      break;
    }
    case Type::U_UNREGISTER: {
      auto derived = std::dynamic_pointer_cast<UserMessage>(base);
      auto user = derived->getUser();
      if (Model::deleteUser(user)) {
        UserMessage newMsg(clientId, user);
        sender->sendMsg(Type::U_UNREGISTER_OK, newMsg);
      } else {
        BasicMessage msg(clientId);
        sender->sendMsg(Type::U_UNREGISTER_KO, msg);
      }
      break;
    }
    case Type::F_CREATE:
    case Type::F_OPEN: {
      auto filename =
          std::dynamic_pointer_cast<RequestMessage>(base)->getFilename();
      FileText symbolList;

      if (header.getType() == Type::F_OPEN &&
          model->openFileByUser(sender, filename)) {
        symbolList = model->getFileBySocket(sender)->getFileText();
      } else if (header.getType() == Type::F_CREATE &&
                 !model->createFileByUser(sender, filename)) {
        BasicMessage newMsg(clientId);
        sender->sendMsg(Type::F_FILE_KO, newMsg);
        break;
      }

      File file(filename, symbolList);
      FileMessage newMsg2(clientId, file);
      sender->sendMsg(Type::F_FILE_OK, newMsg2);
      dispatch(sender, Type::U_CONNECTED,Header(),
              std::make_shared<UserMessage>(clientId, model->getUserActivity(sender)));
      break;
    }
    case Type::S_UPDATE_ATTRIBUTE: {
      auto derived = std::dynamic_pointer_cast<CrdtMessage>(base);
      model->userReplace(sender, derived->getSymbol());
      dispatch(sender, header.getType(), header, derived);
    }
    default:
      throw std::runtime_error("Must never read different types of Message!!!");
    }
  }
  if (sender->isMessageAvailable()) {
    onReadyRead();
  }
}

void Controller::dispatch(TcpSocket* sender, Type headerType, Header header,
        std::shared_ptr<BasicMessage> message) {
  auto serverFile = model->getFileBySocket(sender);
  if (serverFile == nullptr)
    return;
  auto fileConnections = model->getFileConnections(serverFile->getFileName());
  if (!fileConnections.empty()) {
    std::for_each(fileConnections.begin(), fileConnections.end(),
                  [&](auto &socket) {
                      if (socket != sender) {
                        if (header.getType() == headerType) {
                          socket->sendMsg(header, *message);
                        } else {
                          socket->sendMsg(headerType, *message);
                          if (headerType == Type::U_CONNECTED) {
                            UserMessage remoteUser(socket->getIdentifier(),
                                    model->getUserActivity(socket));
                            sender->sendMsg(headerType, remoteUser);
                          }
                        }
                        spdlog::debug("Dispatched message from {} to {}",
                                      sender->getIdentifier(),
                                      socket->getIdentifier());
                      }
                  });
  }
}