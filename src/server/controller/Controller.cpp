#include <spdlog/spdlog.h>
#include <QHostAddress>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/LoginMessage.h"
#include "src/components/messages/RequestMessage.h"
#include "src/components/messages/FileContentMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/ResultMessage.h"
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

  connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
          this,
          SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

  spdlog::debug("Connected Editor {0:d}", clientId);

  BasicMessage msg(Type::CONNECT, clientId);
  clientSocket->sendMsg(msg);

}

void
Controller::onSocketStateChanged(QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::UnconnectedState ||
      socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
    //TODO : rimuovere il client dalla lista di client per file
    sender->deleteLater();
  }
}

void Controller::onReadyRead() {
  auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
  auto clientId = sender->socketDescriptor();

  std::shared_ptr<BasicMessage> base(sender->readMsg());

  switch (base->getMsgType()) {

    case Type::LOGIN : {
      auto derived = std::dynamic_pointer_cast<LoginMessage>(base);

      //TODO : Check into the DATABASE
      bool result =
              derived->getUsername() == "hello" && derived->getPassword() ==
                                                   "9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043";

      ResultMessage newMsg(Type::LOGIN_RESULT, clientId, result);
      sender->sendMsg(newMsg);

      if (result) {
        FileListingMessage newMsg2(clientId, model->getAvailableFiles());
        sender->sendMsg(newMsg2);
      }
      break;
    }
    case Type::INSERT :
    case Type::ERASE : {
      auto derived = std::dynamic_pointer_cast<CrdtMessage>(base);
      base->getMsgType() == Type::INSERT ? model->userInsert(clientId,
                                                             derived->getSymbol())
                                         : model->userErase(clientId,
                                                            derived->getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(derived);
      break;
    }
    case Type::CREATE :
    case Type::OPEN : {
      auto derived = std::dynamic_pointer_cast<RequestMessage>(base);
      QVector<Symbol> symbolList;

      if (base->getMsgType() == Type::OPEN &&
          model->openFileByUser(clientId, derived->getFilename())) {
        symbolList = model->getFileSymbolList(clientId);
      } else if (base->getMsgType() == Type::CREATE &&
                 !model->createFileByUser(clientId, derived->getFilename())) {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, false);
        sender->sendMsg(newMsg);
        break;
      }

      ResultMessage newMsg(Type::FILE_RESULT, clientId, true);
      FileContentMessage newMsg2(clientId, symbolList);
      sender->sendMsg(newMsg);
      sender->sendMsg(newMsg2);
      break;
    }
    default :
      throw std::runtime_error("Must never read different types of Message!!!");
  }
  //TODO : Send also message size to read correctly
  if (sender->bytesAvailable()) {
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