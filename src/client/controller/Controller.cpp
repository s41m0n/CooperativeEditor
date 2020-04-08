#include "Controller.h"

#include <utility>

Controller::Controller(Model *model, const std::string &host, int port)
    : model(model), socket(new TcpSocket(this)) {
  socket->connectToHost(QHostAddress(host.c_str()), port);
  connect(socket, &TcpSocket::messageReceived, this,
          &Controller::onMessageReceived);
  connect(socket, &QTcpSocket::connected, this, &Controller::connected);
  connect(socket, &QTcpSocket::disconnected, this, &Controller::disconnected);
  connect(socket,
          QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
          this, &Controller::error);
}

void Controller::onMessageReceived(Header &header, QByteArray &buf) {

  switch (header.getType()) {
  case Type::U_CONNECT: {
    auto msg = BasicMessage::fromQByteArray(buf);
    model->setEditorId(msg.getEditorId());
    break;
  }
  case Type::U_REGISTER_KO:
  case Type::U_LOGIN_KO: {
    emit loginResponse(false);
    break;
  }
  case Type::U_REGISTER_OK:
  case Type::U_LOGIN_OK: {
    emit loginResponse(true);
    auto msg = UserMessage::fromQByteArray(buf);
    model->setCurrentUser(msg.getUser());
    break;
  }
  case Type::F_LISTING: {
    auto msg = FileListingMessage::fromQByteArray(buf);
    emit fileListing(msg.getFiles());
    break;
  }
  case Type::U_INSERT_INVITE_KO:
  case Type::F_FILE_KO: {
    emit fileResult(false);
    break;
  }
  case Type::U_INSERT_INVITE_OK:
  case Type::F_FILE_OK: {
    emit fileResult(true);
    auto msg = FileMessage::fromQByteArray(buf);
    model->setCurrentFile(msg.getFile());
    emit loadFileText(model->getFileText(), model->getFile().getFileName(),
                      model->getUser().getUsername(), model->getEditorId());
    break;
  }
  case Type::S_INSERT:
  case Type::S_ERASE: {
    try {
      auto msg = CrdtMessage::fromQByteArray(buf);
      auto symbol = msg.getSymbol();
      if (header.getType() == Type::S_INSERT) {
        emit remoteUserInsert(model->remoteInsert(symbol), symbol);
      } else{
        emit remoteUserDelete(model->remoteErase(symbol));
      }
    } catch (std::exception &e) {
      spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
    }
    break;
  }
  case Type::U_CONNECTED: {
    auto msg = UserMessage::fromQByteArray(buf);
    auto userConnected = msg.getUser();
    emit remoteUserConnected(msg.getEditorId(), userConnected.getName());
    break;
  }
  case Type::U_DISCONNECTED: {
    auto msg = BasicMessage::fromQByteArray(buf);
    emit remoteUserDisconnected(msg.getEditorId());
    break;
  }
  case Type::U_GENERATE_INVITE: {
    auto msg = RequestMessage::fromQByteArray(buf);
    emit generateLinkAnswer(msg.getFilename());
    break;
  }
  default:
    emit error();
  }
}

void Controller::prepareToSend(Type type, BasicMessage &msg) {
  auto buf = BasicMessage::toQByteArray(msg);
  Header header(buf.size(), type);
  socket->sendMsg(header, buf);
}

void Controller::onCharInserted(int index, QChar value,
                                QTextCharFormat format) {

  try {
    CrdtMessage msg(model->localInsert(index, value, format),
                    model->getEditorId());
    prepareToSend(Type::S_INSERT, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local insert:\nIndex-> {}\nMsg -> {}", index,
                  e.what());
  }
}

void Controller::onCharErased(int index) {

  try {
    CrdtMessage msg(model->localErase(index), model->getEditorId());
    prepareToSend(Type::S_ERASE, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local erase: Index-> {} @ Msg -> {}", index,
                  e.what());
  }
}

void Controller::onLoginRequest(const QString &username, const QString &password) {

  QByteArray hashedPassword =
      QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha512);

  UserMessage msg(model->getEditorId(),
                  User(username, QString(hashedPassword.toHex())));
  prepareToSend(Type::U_LOGIN, msg);
}

void Controller::onSignUpRequest(User user) {

  UserMessage msg(model->getEditorId(),std::move(user));
  prepareToSend(Type::U_REGISTER, msg);
}

void Controller::onFileRequest(const QString &filename, bool exists) {

  RequestMessage msg(model->getEditorId(), filename);
  prepareToSend(exists ? Type::F_OPEN : Type::F_CREATE, msg);
}

void Controller::onShowEditProfile() {
  User user = model->getUser();
  emit userProfileInfo(user.getPicture(), user.getName(), user.getSurname(),
                       user.getEmail(), user.getUsername());
}

void Controller::onFileClosed() {
  BasicMessage msg(model->getEditorId());
  prepareToSend(Type::U_DISCONNECTED, msg);
}

void Controller::onInsertInviteCode(QString code) {
  RequestMessage msg(model->getEditorId(), std::move(code));
  prepareToSend(Type::U_INSERT_INVITE, msg);
}

void Controller::onGenerateLink() {
  RequestMessage msg(model->getEditorId(), model->getFile().getFileName());
  prepareToSend(Type::U_GENERATE_INVITE, msg);
}