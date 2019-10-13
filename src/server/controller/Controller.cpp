#include <spdlog/spdlog.h>
#include <QHostAddress>
#include <QAbstractSocket>

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


  std::lock_guard<std::mutex> guard(connectionsMutex);

  connections.insert(std::make_pair(clientId, clientSocket));

  BasicMessage msg(Type::CONNECT, clientId);

  clientSocket->sendMsg(msg);
  spdlog::debug("Sent Message:\n" + msg.toString());

}

void
Controller::onSocketStateChanged(QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::UnconnectedState ||
      socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<QTcpSocket *>(QObject::sender());
    std::lock_guard<std::mutex> guard(connectionsMutex);
    sender->deleteLater();
    connections.erase(sender->socketDescriptor());
  }
}

void Controller::onReadyRead() {
  auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
  auto clientId = sender->socketDescriptor();

  BasicMessage *base = sender->readMsg();

  switch (base->getMsgType()) {

    case Type::LOGIN : {
      auto derived = dynamic_cast<LoginMessage*>(base);
      spdlog::debug("Received Message!\n" + derived->toString());

      //TEMPORARY METHOD TO CHECK USER LOGIN PERMISSION
      bool result = derived->getUsername() == "hello" && derived->getPassword() ==
                                                    "9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043";

      ResultMessage newMsg(Type::LOGIN_RESULT, clientId, result);
      sender->sendMsg(newMsg);
      spdlog::debug("Sent Message!\n" + newMsg.toString());

      if (result) {
        FileListingMessage newMsg2(clientId, model->getAvailableFiles());
        sender->sendMsg(newMsg2);
        spdlog::debug("Sent Message:\n" + newMsg2.toString());
      }
      delete base;
      break;
    }
    case Type::INSERT : {
      auto derived = dynamic_cast<CrdtMessage*>(base);
      spdlog::debug("Received Message!\n" + derived->toString());

      model->userInsert(clientId, derived->getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(derived);
      break;
    }
    case Type::ERASE : {
      auto derived = dynamic_cast<CrdtMessage*>(base);
      spdlog::debug("Received Message!\n" + derived->toString());

      model->userErase(clientId, derived->getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(derived);
      break;
    }
    case Type::CREATE : {
      auto derived = dynamic_cast<RequestMessage*>(base);
      spdlog::debug("Received Message!\n" + derived->toString());

      if (model->createFileByUser(clientId, derived->getFilename())) {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, true);
        sender->sendMsg(newMsg);
        spdlog::debug("Sent Message!\n" + newMsg.toString());

        std::vector<Symbol> empty;
        FileContentMessage newMsg2(clientId, empty);
        sender->sendMsg(newMsg2);
        spdlog::debug("Sent Message!\n" + newMsg2.toString());
      } else {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, false);
        sender->sendMsg(newMsg);
        spdlog::debug("Sent Message!\n" + newMsg.toString());
      }
      delete base;
      break;
    }
    case Type::OPEN : {
      auto derived = dynamic_cast<RequestMessage*>(base);
      spdlog::debug("Received Message!\n" + derived->toString());

      if (model->openFileByUser(clientId, derived->getFilename())) {
        auto symbolList = model->getFileSymbolList(clientId);
        ResultMessage newMsg(Type::FILE_RESULT, clientId, true);
        sender->sendMsg(newMsg);
        spdlog::debug("Sent Message!\n" + newMsg.toString());

        FileContentMessage newMsg2(clientId, symbolList);
        sender->sendMsg(newMsg2);
        spdlog::debug("Sent Message!\n" + newMsg2.toString());
      } else {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, false);
        sender->sendMsg(newMsg);
        spdlog::debug("Sent Message!\n{}", newMsg.toString());
      }
      delete base;
      break;
    }
    default :
      throw std::runtime_error("Must never read different types of Message!!!");
  }
  //TEMPORARY SOLUTION
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