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
  if (!worker->setSocketDescriptor(handle)) {
    worker->deleteLater();
    return;
  }
  worker->setClientID(handle);

  connect(worker, &TcpSocket::messageReceived, this,
          &Controller::onMessageReceived);
  connect(worker, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
          SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

  spdlog::debug("Connected Editor {0:d}", handle);

  BasicMessage msg(handle);
  prepareToSend(worker, Type::U_CONNECT, msg);
}

void Controller::onSocketStateChanged(
    QAbstractSocket::SocketState socketState) {
  if (socketState == QAbstractSocket::ClosingState) {
    auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
    BasicMessage msg(sender->getClientID());
    dispatch(sender, Type::U_DISCONNECTED, Header(), msg);
    model->removeUserActivity(sender);
    model->removeConnection(sender);
    spdlog::debug("Disconnected Editor {0:d}", sender->getClientID());
    sender->deleteLater();
  }
}

void Controller::onMessageReceived(Header &header, QByteArray &buf) {
  auto sender = dynamic_cast<TcpSocket *>(QObject::sender());
  auto clientId = sender->getClientID();

  switch (header.getType()) {
  case Type::U_REGISTER:
  case Type::U_LOGIN: {
    auto msg = UserMessage::fromQByteArray(buf);
    auto user = msg.getUser();
    bool result = header.getType() == Type::U_LOGIN ? Model::logInUser(user)
                                                    : Model::registerUser(user);
    if (result) {
      model->insertUserActivity(sender, user);
      UserMessage newMsg(clientId, user);
      prepareToSend(sender,
                    header.getType() == Type::U_LOGIN ? Type::U_LOGIN_OK
                                                      : Type::U_REGISTER_OK,
                    newMsg);
    } else {
      BasicMessage newMsg(clientId);
      prepareToSend(sender,
                    header.getType() == Type::U_LOGIN ? Type::U_LOGIN_KO
                                                      : Type::U_REGISTER_KO,
                    newMsg);
    }
    break;
  }
  case Type::F_LISTING: {
    FileListingMessage newMsg2(clientId, model->getAvailableUserFiles(sender));
    prepareToSend(sender, Type::F_LISTING, newMsg2);
    break;
  }
  case Type::S_INSERT:
  case Type::S_ERASE: {
    auto msg = CrdtMessage::fromQByteArray(buf);
    try {
      if (header.getType() == Type::S_INSERT) {
        model->userInsert(sender, msg.getSymbol());
      } else {
        model->userErase(sender, msg.getSymbol());
      }
      dispatch(sender, header.getType(), header, msg);
    } catch (std::exception &e) {
      spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
    }
    break;
  }
  case Type::U_UPDATE: {
    auto msg = UserMessage::fromQByteArray(buf);
    auto user = msg.getUser();
    if (Model::updateUser(user)) {
      UserMessage newMsg(clientId, user);
      prepareToSend(sender, Type::U_UPDATE_OK, newMsg);
    } else {
      BasicMessage newMsg(clientId);
      prepareToSend(sender, Type::U_UPDATE_KO, newMsg);
    }
    break;
  }
  case Type::F_CREATE:
  case Type::F_OPEN: {
    auto msg = RequestMessage::fromQByteArray(buf);
    auto filename = msg.getResource();
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
    prepareToSend(sender, Type::F_FILE_OK, newMsg);

    UserMessage newMsg2(clientId, model->getUserActivity(sender));
    dispatch(sender, Type::U_CONNECTED, Header(), newMsg2);
    break;
  }
  case Type::U_DISCONNECTED: {
    auto msg = BasicMessage::fromQByteArray(buf);
    dispatch(sender, header.getType(), header, msg);
    model->removeConnection(sender);
    break;
  }
  case Type::U_GENERATE_INVITE: {
    auto msg = RequestMessage::fromQByteArray(buf);
    auto invite = model->generateInvite(sender, msg.getResource());
    RequestMessage newMsg(clientId, invite);
    prepareToSend(sender, Type::U_GENERATE_INVITE, newMsg);
    break;
  }
  case Type::U_INSERT_INVITE: {
    auto msg = RequestMessage::fromQByteArray(buf);
    File file;
    if (model->insertInviteCode(sender, msg.getResource(), file)) {
      FileMessage newMsg(clientId, file);
      prepareToSend(sender, Type::U_INSERT_INVITE_OK, newMsg);
      UserMessage newMsg2(clientId, model->getUserActivity(sender));
      dispatch(sender, Type::U_CONNECTED, Header(), newMsg2);
    } else {
      BasicMessage newMsg(clientId);
      prepareToSend(sender, Type::U_INSERT_INVITE_KO, newMsg);
    }
    break;
  }
  case Type::U_CURSOR: {
    auto msg = CursorMessage::fromQByteArray(buf);
    dispatch(sender, header.getType(), header, msg);
    break;
  }
  default:
    throw std::runtime_error("Must never read different types of Message!!!");
  }
}

void Controller::prepareToSend(TcpSocket *sender, Type type,
                               BasicMessage &msg) {
  auto buf = BasicMessage::toQByteArray(msg);
  Header header(buf.size(), type);
  sender->sendMsg(header, buf);
}

void Controller::dispatch(TcpSocket *sender, Type headerType, Header header,
                          BasicMessage &message) {
  auto serverFile = model->getFileBySocket(sender);
  if (serverFile == nullptr)
    return;
  auto fileConnections = model->getFileConnections(serverFile->getFileID());
  if (!fileConnections.empty()) {
    // Serializing only once the message to forward and directly call
    // socket->sendMsg instead of this->preparingMsg
    auto buf = BasicMessage::toQByteArray(message);
    auto head = header.getType() == headerType ? header
                                               : Header(buf.size(), headerType);
    std::for_each(
        fileConnections.begin(), fileConnections.end(), [&](auto &socket) {
          if (socket != sender) {
            socket->sendMsg(head, buf);
            if (headerType == Type::U_CONNECTED) {
              UserMessage remoteUser(socket->getClientID(),
                                     model->getUserActivity(socket));
              prepareToSend(sender, headerType, remoteUser);
            }
            spdlog::debug("Dispatched message from {} to {}",
                          sender->getClientID(), socket->getClientID());
          }
        });
  }
}