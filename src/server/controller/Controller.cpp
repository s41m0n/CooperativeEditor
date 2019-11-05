#include <spdlog/spdlog.h>
#include <QHostAddress>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/RequestMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "Controller.h"

Controller::Controller(Model *model, unsigned short port, QWidget *parent)
        : QTcpServer(parent),
          model(model) {
  if(!listen(QHostAddress::Any, port)) {
    spdlog::error("Unable to listen on address {} and port {}", QHostAddress::Any, port);
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
          this,
          SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

  spdlog::debug("Connected Editor {0:d}", clientId);

  BasicMessage msg(clientId);
  clientSocket->sendMsg(Type::CONNECT, msg);

}

void
Controller::onSocketStateChanged(QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
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

      case Type::REGISTER :
      case Type::LOGIN : {
        auto derived = std::dynamic_pointer_cast<UserMessage>(base);

        auto user = derived->getUser();

        bool result = header.getType() == Type::LOGIN? model->logInUser(user) : model->registerUser(user);

        if (result) {
          UserMessage newMsg(clientId, user);
          FileListingMessage newMsg2(clientId, model->getAvailableFiles());
          sender->sendMsg(header.getType() == Type::LOGIN? Type::LOGIN_OK : Type::REGISTER_OK, newMsg);
          sender->sendMsg(Type::LISTING, newMsg2);
        } else {
          BasicMessage msg(clientId);
          sender->sendMsg(header.getType() == Type::LOGIN? Type::LOGIN_KO : Type::REGISTER_KO, msg);
        }
        break;
      }
      case Type::INSERT :
      case Type::ERASE : {
        auto derived = std::dynamic_pointer_cast<CrdtMessage>(base);
        try {
          header.getType() == Type::INSERT ? model->userInsert(
                  sender,derived->getSymbol())
                                           : model->userErase(
                  sender,derived->getSymbol());
          auto fileName = model->getFileBySocket(sender).getFileName();
          auto fileConnections = model->getFileConnections(fileName);
          if (!fileConnections.empty()) {
            std::for_each(fileConnections.begin(), fileConnections.end(),
                    [&](auto& socket){
                      if (socket != sender) {
                        socket->sendMsg(header, *derived);
                        spdlog::debug("Dispatched message from {} to {}", sender->getIdentifier(), socket->getIdentifier());
                      }
                    });
          }
        } catch (std::exception &e) {
          spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
        }
        break;
      }
      case Type::CREATE :
      case Type::OPEN : {
        auto filename = std::dynamic_pointer_cast<RequestMessage>(
                base)->getFilename();
        FileText symbolList;

        if (header.getType() == Type::OPEN &&
            model->openFileByUser(sender, filename)) {
          symbolList = model->getFileBySocket(sender).getFileText();
        } else if (header.getType() == Type::CREATE &&
                   !model->createFileByUser(sender, filename)) {
          BasicMessage newMsg(clientId);
          sender->sendMsg(Type::FILE_KO, newMsg);
          break;
        }

        File file(filename, symbolList);
        FileMessage newMsg2(clientId, file);
        sender->sendMsg(Type::FILE_OK, newMsg2);
        break;
      }
      default :
        throw std::runtime_error(
                "Must never read different types of Message!!!");
    }
  }
  if (sender->isMessageAvailable()) {
    onReadyRead();
  }
}