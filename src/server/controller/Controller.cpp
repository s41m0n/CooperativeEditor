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

  *clientSocket << msg;
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

  BasicMessage base;
  *sender >> base;

  switch (base.getMsgType()) {

    case Type::LOGIN : {
      LoginMessage msg(std::move(base));
      *sender >> msg;
      spdlog::debug("Received Message!\n" + msg.toString());

      //TEMPORARY METHOD TO CHECK USER LOGIN PERMISSION
      bool result = msg.getUsername() == "hello" && msg.getPassword() ==
                                                    "9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043";

      ResultMessage newMsg(Type::LOGIN_RESULT, clientId, result);
      *sender << newMsg;
      spdlog::debug("Sent Message!\n" + newMsg.toString());

      if (result) {
        FileListingMessage newMsg2(clientId,
                                   model->getAvailableFiles());
        *sender << newMsg2;
        spdlog::debug("Sent Message:\n" + newMsg2.toString());
      }
      break;
    }
    case Type::INSERT : {
      CrdtMessage msg(std::move(base));
      *sender >> msg;
      spdlog::debug("Received Message!\n" + msg.toString());

      model->userInsert(clientId, msg.getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(std::move(msg));
      break;
    }
    case Type::ERASE : {
      CrdtMessage msg(std::move(base));
      *sender >> msg;
      spdlog::debug("Received Message!\n" + msg.toString());

      model->userErase(clientId, msg.getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(std::move(msg));
      break;
    }
    case Type::CREATE : {
      RequestMessage msg(std::move(base));
      *sender >> msg;
      spdlog::debug("Received Message!\n" + msg.toString());

      if (model->createFileByUser(clientId, msg.getFilename())) {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, true);
        std::vector<Symbol> empty;
        FileContentMessage newMsg2(clientId, empty);
        *sender << newMsg;
        *sender << newMsg2;
        spdlog::debug("Sent Message!\n" + newMsg.toString());
        spdlog::debug("Sent Message!\n" + newMsg2.toString());
      } else {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, false);
        *sender << newMsg;
        spdlog::debug("Sent Message!\n" + newMsg.toString());
      }
      break;
    }
    case Type::OPEN : {
      RequestMessage msg(std::move(base));
      *sender >> msg;
      spdlog::debug("Received Message!\n" + msg.toString());

      if (model->openFileByUser(clientId, msg.getFilename())) {
        auto symbolList = model->getFileSymbolList(clientId);
        ResultMessage newMsg(Type::FILE_RESULT, clientId, true);
        FileContentMessage newMsg2(clientId, symbolList);
        *sender << newMsg;
        *sender << newMsg2;
        spdlog::debug("Sent Message!\n" + newMsg.toString());
        spdlog::debug("Sent Message!\n" + newMsg2.toString());
      } else {
        ResultMessage newMsg(Type::FILE_RESULT, clientId, false);
        *sender << newMsg;
        spdlog::debug("Sent Message!\n{}", newMsg.toString());
      }
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