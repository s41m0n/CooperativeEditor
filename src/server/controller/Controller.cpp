//
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <memory>
#include <QHostAddress>
#include <QAbstractSocket>

#include "utility/CrdtAlgorithm.h"
#include "Controller.h"

Controller::Controller(Model *model, unsigned short port) : model(model), _server(this) {
  spdlog::debug("Created Controller");
  _server.listen(QHostAddress::Any, port);
  connect(&_server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

Controller::~Controller() {
  spdlog::debug("Destroyed Controller");
}

void Controller::onNewConnection() {

  auto clientSocket = _server.nextPendingConnection();
  auto clientId = model->generateEditorId();

  spdlog::debug("Connected Editor{0:d}", clientId);

  connect(clientSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(clientSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
          SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

  std::lock_guard<std::mutex> guard(connectionsMutex);
  connections.insert(std::pair<QTcpSocket *, unsigned int>(clientSocket, clientId));

  //Sending Connect and FileListing Message
  BasicMessage msg(Type::CONNECT, clientId);
  RequestMessage msg2(Type::LISTING, clientId, model->getAvailableFiles());
  QDataStream ds(clientSocket);
  ds << msg << msg2;
  spdlog::debug("Sent Message:\n" + msg.toString());
  spdlog::debug("Sent Message:\n" + msg2.toString());
}

void Controller::onSocketStateChanged(QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::UnconnectedState) {
    auto sender = dynamic_cast<QTcpSocket *>(QObject::sender());
    std::lock_guard<std::mutex> guard(connectionsMutex);
    connections.erase(sender);
  }
}

void Controller::onReadyRead() {
  auto sender = dynamic_cast<QTcpSocket *>(QObject::sender());
  auto clientId = connections[sender];
  QDataStream ds(sender);
  BasicMessage base;
  ds >> base;


  switch (base.getMsgType()) {

    case Type::INSERT : {
      CrdtMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n"+ msg.toString());

      model->userInsert(clientId, msg.getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(msg);
      break;
    }
    case Type::ERASE : {
      CrdtMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n"+ msg.toString());

      model->userErase(clientId, msg.getSymbol());
      std::lock_guard<std::mutex> guard2(queueMutex);
      messages.push(msg);
      break;
    }
    case Type::CREATE : {
      RequestMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n"+ msg.toString());

      auto filename = msg.getFilename();
      if (model->createFileByUser(clientId, filename)) {
        RequestMessage newMsg(Type::FILEOK, clientId, filename);
        std::vector<Symbol> empty;
        FileContentMessage newMsg2(Type::CONTENT, clientId, empty);
        ds << newMsg << newMsg2;
        spdlog::debug("Sent Message!\n"+ newMsg.toString());
        spdlog::debug("Sent Message!\n"+ newMsg2.toString());
      } else {
        RequestMessage newMsg(Type::FILEKO, clientId, filename);
        ds << newMsg;
        spdlog::debug("Sent Message!\n"+ newMsg.toString());
      }
      break;
    }
    case Type::OPEN : {
      RequestMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n"+ msg.toString());

      auto filename = msg.getFilename();

      if (model->openFileByUser(clientId, filename)) {
        auto symbolList = model->getFileSymbolList(clientId);
        RequestMessage newMsg(Type::FILEOK, clientId, filename);
        FileContentMessage newMsg2(Type::CONTENT, clientId, symbolList);
        ds << newMsg << newMsg2;
        spdlog::debug("Sent Message!\n"+ newMsg.toString());
        spdlog::debug("Sent Message!\n"+ newMsg2.toString());
      } else {
        RequestMessage newMsg(Type::FILEKO, clientId, filename);
        ds << newMsg;
        spdlog::debug("Sent Message!\n{}", newMsg.toString());
      }
      break;
    }
    default :
      throw std::runtime_error("Must never read different types of Message!!!");
  }
  if (sender->bytesAvailable())
    onReadyRead();
}


void Controller::dispatch() {

  while (connections.size() >= 2) {

    std::lock_guard<std::mutex> guard1(queueMutex);

    for (int i = 0, size = this->messages.size(); i < size; i++) {

      auto msg = this->messages.front();
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
