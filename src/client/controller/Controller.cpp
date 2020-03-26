#include <QCryptographicHash>
#include <QHostAddress>
#include <QImage>
#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

#include "Controller.h"
#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/UserMessage.h"

Controller::Controller(const std::string &host, int port)
    : model(new Model()), socket(new TcpSocket(this)) {
  socket->connectToHost(QHostAddress(host.c_str()), port);
  connect(socket, &TcpSocket::messageReceived, this,
          &Controller::onMessageReceived);
  connect(socket, &QTcpSocket::connected, this, &Controller::connected);
  connect(socket, &QTcpSocket::disconnected, this, &Controller::disconnected);
  // Forward the error signal, QOverload is necessary as error() is overloaded,
  // see the Qt docs
  connect(socket,
          QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
          this, &Controller::error);
}

void Controller::onMessageReceived(Header &header, QByteArray &buf) {

  QDataStream ds(&buf, QIODevice::ReadOnly);
  switch (header.getType()) {
  case Type::U_CONNECT: {
    BasicMessage msg;
    ds >> msg;
    model->setEditorId(msg.getEditorId());
    socket->setIdentifier(msg.getEditorId());
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
    UserMessage msg;
    ds >> msg;
    model->setCurrentUser(msg.getUser());
    break;
  }
  case Type::F_LISTING: {
    FileListingMessage msg;
    ds >> msg;
    emit fileListing(msg.getFiles());
    break;
  }
  case Type::F_FILE_KO: {
    emit fileResult(false);
    break;
  }
  case Type::F_FILE_OK: {
    emit fileResult(true);
    FileMessage msg;
    ds >> msg;
    model->setCurrentFile(msg.getFile());
    emit loadFileText(model->getFileText(), model->getFile().getFileName(),
                      model->getUser(), model->getEditorId());
    break;
  }
  case Type::S_UPDATE_ATTRIBUTE:
  case Type::S_INSERT:
  case Type::S_ERASE: {
    try {
      CrdtMessage msg;
      ds >> msg;
      auto symbols = msg.getSymbols();
      if (header.getType() == Type::S_INSERT) {
        emit remoteUserInsert(model->remoteInsert(symbols), symbols);
      } else if (header.getType() == Type::S_ERASE) {
        emit remoteUserDelete(model->remoteErase(symbols), symbols.size());
      } else {
        emit remoteUserUpdate(model->remoteUpdate(symbols), symbols);
      }
    } catch (std::exception &e) {
      spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
    }
    break;
  }
  case Type::U_CONNECTED: {
    UserMessage msg;
    ds >> msg;
    auto userConnected = msg.getUser();
    emit remoteUserConnected(
        msg.getEditorId(), userConnected.getName());
    break;
  }
  case Type::U_DISCONNECTED: {
    BasicMessage msg;
    ds >> msg;
    emit remoteUserDisconnected(msg.getEditorId());
    break;
  }
  default:
    throw std::runtime_error("Unknown message received\n" +
                             header.toStdString());
  }
}

void Controller::prepareToSend(Type type, BasicMessage &msg) {
  QByteArray buf;
  QDataStream ds(&buf, QIODevice::WriteOnly);
  ds << msg;
  Header header(buf.size(), type);
  spdlog::info("Sending message:\n{}\n{}", header.toStdString(),
               msg.toStdString());
  socket->sendMsg(header, buf);
}

void Controller::onCharInserted(int index, const QString &value,
                                const QVector<bool> &attributes) {

  try {

    CrdtMessage msg(model->localInsert(index, value, attributes),
                    model->getEditorId());
    prepareToSend(Type::S_INSERT, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local insert:\nIndex-> {}\nMsg -> {}", index,
                  e.what());
  }
}

void Controller::onCharErased(int index, int size) {

  try {
    CrdtMessage msg(model->localErase(index, size), model->getEditorId());
    prepareToSend(Type::S_ERASE, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local erase: Index-> {} @ Msg -> {}", index,
                  e.what());
  }
}

void Controller::onCharUpdated(int index, int size, Attribute attribute,
                               bool set) {
  try {
    CrdtMessage msg(model->localUpdate(index, size, attribute, set),
                    model->getEditorId());
    prepareToSend(Type::S_UPDATE_ATTRIBUTE, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local update: Index-> {} @ Msg -> {}", index,
                  e.what());
  }
}

void Controller::onLoginRequest(const QString &username,
                                const QString &password) {

  QByteArray hashedPassword =
      QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha512);

  UserMessage msg(model->getEditorId(),
                  User(username, QString(hashedPassword.toHex())));
  prepareToSend(Type::U_LOGIN, msg);
}

void Controller::onSignUpRequest(QImage image, QString name, QString surname,
                                 QString username, QString email,
                                 const QString &password) {

  QByteArray hashedPassword =
      QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha512);

  UserMessage msg(model->getEditorId(),
                  User(std::move(username), std::move(name), std::move(surname),
                       std::move(email), QString(hashedPassword.toHex()),
                       std::move(image)));
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
