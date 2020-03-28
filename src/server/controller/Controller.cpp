#include "Controller.h"

Controller::Controller(Model *model, unsigned short port, QWidget *parent)
    : QTcpServer(parent), model(model) {
  if (!listen(QHostAddress::Any, port)) {
    spdlog::error("Unable to listen on address {} and port {}",
                  QHostAddress::Any, port);
    exit(-1);
  }
}

void Controller::incomingConnection(qintptr handle) {
  auto worker = new TcpSocket(this);
  worker->setSocketDescriptor(handle);

  connect(worker, &TcpSocket::messageReceived, this,
          &Controller::onMessageReceived);
  connect(worker, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
          this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

  spdlog::debug("Connected Editor {0:d}", handle);

  BasicMessage msg(handle);
  prepareToSend(worker, Type::U_CONNECT, msg);
}
void Controller::prepareToSend(TcpSocket *sender, Type type,
                               BasicMessage &msg) {
  QByteArray buf;
  QDataStream ds(&buf, QIODevice::WriteOnly);
  ds << msg;
  Header header(buf.size(), type);
  sender->sendMsg(header, buf);
  spdlog::info("Sending message:\n{}\n{}", header.toStdString(), msg.toStdString());
}

void Controller::onSocketStateChanged(
    QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
    BasicMessage msg(sender->socketDescriptor());
    dispatch(sender, Type::U_DISCONNECTED, Header(), msg);
    model->removeUserActivity(sender);
    model->removeConnection(sender);
    sender->deleteLater();
  }
}

void Controller::onMessageReceived(Header &header, QByteArray &buf) {
  auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
  auto clientId = sender->socketDescriptor();
  QDataStream ds(&buf, QIODevice::ReadOnly);

  switch (header.getType()) {
  case Type::U_REGISTER:
  case Type::U_LOGIN: {
    UserMessage msg;
    ds >> msg;
    auto user = msg.getUser();
    bool result = header.getType() == Type::U_LOGIN ? Model::logInUser(user)
                                                    : Model::registerUser(user);
    if (result) {
      spdlog::error("Son qui");
      UserMessage newMsg(clientId, user);
      FileListingMessage newMsg2(clientId, model->getAvailableFiles());
      prepareToSend(sender,
                    header.getType() == Type::U_LOGIN ? Type::U_LOGIN_OK
                                                      : Type::U_REGISTER_OK,
                    newMsg);
      model->insertUserActivity(sender, user);
      prepareToSend(sender, Type::F_LISTING, newMsg2);
    } else {
      BasicMessage newMsg(clientId);
      prepareToSend(sender,
                    header.getType() == Type::U_LOGIN ? Type::U_LOGIN_KO
                                                      : Type::U_REGISTER_KO,
                    newMsg);
    }
    break;
  }
  case Type::S_INSERT:
  case Type::S_ERASE: {
    CrdtMessage msg;
    ds >> msg;
    try {
      header.getType() == Type::S_INSERT
          ? model->userInsert(sender, msg.getSymbols())
          : model->userErase(sender, msg.getSymbols());
      dispatch(sender, header.getType(), header, msg);
    } catch (std::exception &e) {
      spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
    }
    break;
  }
  case Type::U_UPDATE: {
    UserMessage msg;
    ds >> msg;
    auto user = msg.getUser();
    if (Model::updateUser(user)) {
      UserMessage newMsg(clientId, user);
      prepareToSend(sender, Type::U_UPDATE_OK, newMsg);
    } else {
      BasicMessage newMsg(clientId);
      prepareToSend(sender, Type::U_UPDATE_KO, msg);
    }
    break;
  }
  case Type::U_UNREGISTER: {
    UserMessage msg;
    ds >> msg;
    auto user = msg.getUser();
    if (Model::deleteUser(user)) {
      UserMessage newMsg(clientId, user);
      prepareToSend(sender, Type::U_UNREGISTER_OK, newMsg);
    } else {
      BasicMessage newMsg(clientId);
      prepareToSend(sender, Type::U_UNREGISTER_KO, msg);
    }
    break;
  }
  case Type::F_CREATE:
  case Type::F_OPEN: {
    RequestMessage msg;
    ds >> msg;
    auto filename = msg.getFilename();
    FileText symbolList;

    if (header.getType() == Type::F_OPEN &&
        model->openFileByUser(sender, filename)) {
      symbolList = model->getFileBySocket(sender)->getFileText();
    } else if (header.getType() == Type::F_CREATE &&
               !model->createFileByUser(sender, filename)) {
      BasicMessage newMsg(clientId);
      prepareToSend(sender, Type::F_FILE_KO, newMsg);
      break;
    }
    File file(filename, symbolList);
    FileMessage newMsg(clientId, file);
    UserMessage newMsg2(clientId, model->getUserActivity(sender));
    prepareToSend(sender, Type::F_FILE_OK, newMsg);
    dispatch(sender, Type::U_CONNECTED, Header(), newMsg2);
    break;
  }
  case Type::S_UPDATE_ATTRIBUTE: {
    CrdtMessage msg;
    ds >> msg;
    model->userReplace(sender, msg.getSymbols());
    dispatch(sender, header.getType(), header, msg);
    break;
  }
  case Type::U_DISCONNECTED: {
    BasicMessage msg;
    ds >> msg;
    dispatch(sender, header.getType(), header, msg);
    model->removeConnection(sender);
    break;
  }
  default:
    throw std::runtime_error("Must never read different types of Message!!!");
  }
}

void Controller::dispatch(TcpSocket *sender, Type headerType, Header header,
                          BasicMessage &message) {
  auto serverFile = model->getFileBySocket(sender);
  if (serverFile == nullptr)
    return;
  auto fileConnections = model->getFileConnections(serverFile->getFileName());
  if (!fileConnections.empty()) {
    std::for_each(
        fileConnections.begin(), fileConnections.end(), [&](auto &socket) {
          if (socket != sender) {
            if (header.getType() == headerType) {
              prepareToSend(socket, header.getType(), message);
            } else {
              prepareToSend(socket, headerType, message);
              if (headerType == Type::U_CONNECTED) {
                UserMessage remoteUser(socket->socketDescriptor(),
                                       model->getUserActivity(socket));
                prepareToSend(socket, headerType, remoteUser);
              }
            }
            spdlog::debug("Dispatched message from {} to {}",
                          sender->socketDescriptor(), socket->socketDescriptor());
          }
        });
  }
}