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
  listen(QHostAddress::Any, port);
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
  if (socketState == QAbstractSocket::UnconnectedState ||
      socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
    model->removeConnection(sender->getIdentifier());
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

      case Type::LOGIN : {
        auto derived = std::dynamic_pointer_cast<UserMessage>(base);

        //TODO : Check into the DATABASE
        auto user = derived->getUser();
        bool result =
                user.getUsername() == "hello" && user.getPassword() ==
                                                 "9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043";

        if (result) {
          User tmp("icon", user.getUsername(), "name", "surname", "email", {});
          UserMessage newMsg(clientId, tmp);
          FileListingMessage newMsg2(clientId, model->getAvailableFiles());
          sender->sendMsg(Type::LOGIN_OK, newMsg);
          sender->sendMsg(Type::LISTING, newMsg2);
        } else {
          BasicMessage msg(clientId);
          sender->sendMsg(Type::LOGIN_KO, msg);
        }
        break;
      }
      case Type::REGISTER : {
        break;
      }
      case Type::INSERT :
      case Type::ERASE : {
        auto derived = std::dynamic_pointer_cast<CrdtMessage>(base);
        try {
          header.getType() == Type::INSERT ? model->userInsert(
                  clientId,
                  derived->getSymbol())
                                           : model->userErase(
                  clientId,
                  derived->getSymbol());
          std::lock_guard<std::mutex> guard2(queueMutex);
          messages.push(derived);
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
            model->openFileByUser(clientId, filename)) {
          symbolList = model->getFileSymbolList(clientId);
        } else if (header.getType() == Type::CREATE &&
                   !model->createFileByUser(clientId, filename)) {
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
/*

void Controller::dispatch() {

  while (connections.size() >= 2) {

    std::lock_guard<std::mutex> guard1(queueMutex);

    for (int i = 0, size = this->messages.size(); i < size; i++) {

      auto &msg = this->messages.front();

      std::lock_guard<std::mutex> guard2(connectionsMutex);

      for (auto &conn : connections) {
        if (conn.second != msg.getEditorId()) {
          QDataStream ds(conn.first);
          ds << msg;
        }
      }
      this->messages.pop();
    }
  }
}
*/