#include "Controller.h"

Controller::Controller(Model *model, const std::string &host, int port)
    : model(model), socket(new TcpSocket(this)) {
  socket->connectToHost(QHostAddress(host.c_str()), port);
  connect(socket, &TcpSocket::messageReceived, this,
          &Controller::onMessageReceived);
  connect(socket, &QTcpSocket::connected, this, &Controller::connected);
  connect(socket, &QTcpSocket::disconnected, this, &Controller::disconnected);
  connect(socket,
          QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
          [&]() -> void {emit error("Socket error");});
}

void Controller::onMessageReceived(Header &header, QByteArray &buf) {

  switch (header.getType()) {
  case Type::U_CONNECT: {
    auto msg = BasicMessage::fromQByteArray(buf);
    model->setEditorId(msg.getEditorId());
    break;
  }
  case Type::U_REGISTER_KO: {
    emit loginResponse(false, true);
    break;
  }
  case Type::U_LOGIN_KO: {
    emit loginResponse(false, false);
    break;
  }
  case Type::U_REGISTER_OK:
  case Type::U_LOGIN_OK: {
    emit loginResponse(true, false);
    auto msg = UserMessage::fromQByteArray(buf);
    model->setCurrentUser(msg.getUser());
    break;
  }
  case Type::U_UPDATE_KO: {
    emit updateResponse(false);
    break;
  }
  case Type::U_UPDATE_OK: {
    auto msg = UserMessage::fromQByteArray(buf);
    model->setCurrentUser(msg.getUser());
    emit updateResponse(true);
    break;
  }
  case Type::F_LISTING: {
    auto msg = FileListingMessage::fromQByteArray(buf);
    emit fileListing(msg.getFiles());
    break;
  }
  case Type::U_INSERT_INVITE_KO: {
    emit fileResult(false, true);
    break;
  }
  case Type::F_FILE_KO: {
    emit fileResult(false, false);
    break;
  }
  case Type::U_INSERT_INVITE_OK:
  case Type::F_FILE_OK: {
    emit fileResult(true, header.getType() == Type::U_INSERT_INVITE_OK);
    auto msg = FileMessage::fromQByteArray(buf);
    model->setCurrentFile(msg.getFile());
    auto file = model->getFile();
    emit loadFileText(file.getFileText(), file.getFileName(),
                      model->getUser().getUsername(), model->getEditorId());
    break;
  }
  case Type::S_INSERT:
  case Type::S_ERASE: {
    try {
      auto msg = CrdtMessage::fromQByteArray(buf);
      auto symbol = msg.getSymbol();
      if (header.getType() == Type::S_INSERT) {
        auto index = model->remoteInsert(symbol);
        if (index >= 0)
          emit remoteUserInsert(index, symbol);
      } else {
        auto index = model->remoteErase(symbol);
        if (index >= 0)
          emit remoteUserDelete(index);
      }
    } catch (std::exception &e) {
      spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
    }
    break;
  }
  case Type::U_CONNECTED: {
    auto msg = UserMessage::fromQByteArray(buf);
    auto userConnected = msg.getUser();
    emit remoteUserConnected(msg.getEditorId(), userConnected.getUsername());
    break;
  }
  case Type::U_DISCONNECTED: {
    auto msg = BasicMessage::fromQByteArray(buf);
    emit remoteUserDisconnected(msg.getEditorId());
    break;
  }
  case Type::U_GENERATE_INVITE: {
    auto msg = RequestMessage::fromQByteArray(buf);
    emit generateLinkAnswer(msg.getResource());
    break;
  }
  case Type::U_CURSOR: {
    auto msg = CursorMessage::fromQByteArray(buf);
    emit userCursorChanged(msg.getEditorId(), msg.getPos());
    break;
  }
  case Type::U_UNAUTHORIZED : {
    emit error("Unauthorized");
    break;
  }
  default:
    emit error("Unknown message");
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
    spdlog::debug("Error on local insert:\nIndex-> {}\nMsg -> {}", index,
                  e.what());
  }
}

void Controller::onCharErased(int index) {

  try {
    CrdtMessage msg(model->localErase(index), model->getEditorId());
    prepareToSend(Type::S_ERASE, msg);
  } catch (std::exception &e) {
    spdlog::debug("Error on local erase: Index-> {} @ Msg -> {}", index,
                  e.what());
  }
}

void Controller::onLoginRequest(User user) {

  UserMessage msg(model->getEditorId(), std::move(user));
  prepareToSend(Type::U_LOGIN, msg);
}

void Controller::onUpdateRequest(User user, const QString& oldPassword) {
  UserUpdateMessage msg(model->getEditorId(), std::move(user), oldPassword.isEmpty()? "" : QCryptographicHash::hash(oldPassword.toUtf8(),
                                                                             QCryptographicHash::Sha512).toHex());
  prepareToSend(Type::U_UPDATE, msg);
}

void Controller::onSignUpRequest(User user) {

  UserMessage msg(model->getEditorId(), std::move(user));
  prepareToSend(Type::U_REGISTER, msg);
}

void Controller::onFileRequest(const QString &filename, bool exists) {

  RequestMessage msg(model->getEditorId(), filename);
  prepareToSend(exists ? Type::F_OPEN : Type::F_CREATE, msg);
}

void Controller::onShowEditProfile() { emit userProfileInfo(model->getUser()); }

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

void Controller::onCursorChanged(int position) {
  CursorMessage msg(model->getEditorId(), position);
  prepareToSend(Type::U_CURSOR, msg);
}

void Controller::onRequestFileList() {
  BasicMessage msg(model->getEditorId());
  prepareToSend(Type::F_LISTING, msg);
}

void Controller::onUserTextAsked(const QString& username) {
  QList<int> userCharsPos;
  for(int i = 0; i < model->getFile().getFileText().size(); i++){
    if(model->getFile().getFileText()[i].getGeneratorUsername() == username){
      userCharsPos.push_back(i);
    }
  }
  emit sendUserText(userCharsPos, username);
}

void Controller::onUserOriginalTextAsked(const QString& username) {
  QMap<int, QBrush> textAndColors;
  for(int i = 0; i < model->getFile().getFileText().size(); i++){
    if(model->getFile().getFileText()[i].getGeneratorUsername() == username){
      textAndColors.insert(i, model->getFile().getFileText()[i].getFormat().background());
    }
  }
  emit sendUserOriginalText(textAndColors);
}
