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

Controller::Controller(Model *model, const std::string &host, int port)
    : model(model) {
  connectToHost(QHostAddress(host.c_str()), port);
  connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void Controller::onReadyRead() {

  while (isMessageAvailable()) {

    auto header = getHeader();
    std::shared_ptr<BasicMessage> base(readMsg());

    switch (header.getType()) {
    case Type::U_CONNECT: {
      model->setEditorId(base->getEditorId());
      setIdentifier(base->getEditorId());
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
      model->setCurrentUser(
          std::dynamic_pointer_cast<UserMessage>(base)->getUser());
      break;
    }
    case Type::F_LISTING: {
      emit fileListing(
          std::dynamic_pointer_cast<FileListingMessage>(base)->getFiles());
      break;
    }
    case Type::F_FILE_KO: {
      emit fileResult(false);
      break;
    }
    case Type::F_FILE_OK: {
      emit fileResult(true);
      model->setCurrentFile(
          std::dynamic_pointer_cast<FileMessage>(base)->getFile());
      emit loadFileText(model->getFileText());
      break;
    }
    case Type::S_INSERT:
    case Type::S_ERASE: {
      try {
        auto symbols = std::dynamic_pointer_cast<CrdtMessage>(base)->getSymbols();
        if (header.getType() == Type::S_INSERT) {
          emit remoteUserInsert(model->remoteInsert(symbols), symbols);
        } else {
          emit remoteUserDelete(model->remoteErase(symbols), symbols.size());
        }
      } catch (std::exception &e) {
        spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
      }
      break;
    }
    case Type::U_CONNECTED: {
      auto userConnected =
              std::dynamic_pointer_cast<UserMessage>(base)->getUser();
      emit remoteUserConnected(base->getEditorId(), userConnected.getPicture(),
              userConnected.getName(), userConnected.getSurname(),
              userConnected.getEmail(), userConnected.getUsername());
      break;
    }
    case Type::U_DISCONNECTED: {
      emit remoteUserDisconnected(base->getEditorId());
      break;
    }
    case Type::S_UPDATE_ATTRIBUTE: {
      auto symbols = std::dynamic_pointer_cast<CrdtMessage>(base)->getSymbols();
      emit remoteUserUpdate(model->remoteUpdate(symbols), symbols);
      break;
    }
    default:
      throw std::runtime_error("Unknown message received");
    }
  }
}

void Controller::onCharInserted(int index, const QString& value, const QVector<bool>& attributes) {

  try {
    CrdtMessage msg(model->localInsert(index, value, attributes), model->getEditorId());
    sendMsg(Type::S_INSERT, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local insert:\nIndex-> {}\nMsg -> {}", index,
                  e.what());
  }
}

void Controller::onCharErased(int index, int size) {

  try {
    CrdtMessage msg(model->localErase(index, size), model->getEditorId());
    sendMsg(Type::S_ERASE, msg);
  } catch (std::exception &e) {
    spdlog::error("Error on local erase: Index-> {} @ Msg -> {}", index,
                  e.what());
  }
}

void Controller::onCharUpdated(int index, int size, const QVector<bool>& attributes) {
  try {
    CrdtMessage msg(model->localUpdate(index, size, attributes), model->getEditorId());
    sendMsg(Type::S_UPDATE_ATTRIBUTE, msg);
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
  sendMsg(Type::U_LOGIN, msg);
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
  sendMsg(Type::U_REGISTER, msg);
}

void Controller::onFileRequest(const QString &filename, bool exists) {

  RequestMessage msg(model->getEditorId(), filename);
  sendMsg(exists ? Type::F_OPEN : Type::F_CREATE, msg);
}

void Controller::onShowEditProfile(){
  User user = model->getUser();
  emit userProfileInfo(user.getPicture(), user.getName(), user.getSurname(),
          user.getEmail(), user.getUsername());
}
