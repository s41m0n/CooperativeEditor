#include <QHostAddress>
#include <QCryptographicHash>
#include <QImage>
#include <memory>
#include <utility>
#include <spdlog/spdlog.h>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/UserMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "Controller.h"

Controller::Controller(Model *model, const std::string &host, int port)
        : model(model) {
  connectToHost(QHostAddress(host.c_str()), port);
  connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void Controller::onReadyRead() {

  if (isMessageAvailable()) {

    std::shared_ptr<BasicMessage> base(readMsg());

    switch (base->getMsgType()) {
      case Type::CONNECT : {
        model->setEditorId(base->getEditorId());
        setIdentifier(base->getEditorId());
        break;
      }
      case Type::LOGIN_KO : {
        emit loginResponse(false);
        break;
      }
      case Type::LOGIN_OK : {
        emit loginResponse(true);
        model->setCurrentUser(
                std::dynamic_pointer_cast<UserMessage>(base)->getUser());
        break;
      }
      case Type::LISTING : {
        emit fileListing(
                std::dynamic_pointer_cast<FileListingMessage>(
                        base)->getFiles());
        break;
      }
      case Type::FILE_KO : {
        emit fileResult(false);
        break;
      }
      case Type::FILE_OK : {
        emit fileResult(true);
        model->setCurrentFile(std::dynamic_pointer_cast<FileMessage>(
                base)->getFile());
        emit remoteUpdate(model->textify());
        break;
      }
      case Type::INSERT :
      case Type::ERASE : {
        try {
          base->getMsgType() == Type::INSERT ? model->remoteInsert(
                  std::dynamic_pointer_cast<CrdtMessage>(base)->getSymbol())
                                             : model->remoteErase(
                  std::dynamic_pointer_cast<CrdtMessage>(base)->getSymbol());
          emit remoteUpdate(model->textify());
        } catch (std::exception &e) {
          spdlog::error("Error on remote operation:\nMsg -> {}", e.what());
        }
        break;
      }
      default :
        throw std::runtime_error("Unknown message received");
    }
  }

}

void Controller::onCharInserted(int index, QChar value) {

  if (state() == QTcpSocket::ConnectedState) {
    try {
      CrdtMessage msg(Type::INSERT, model->localInsert(index, value),
                      model->getEditorId());
      sendMsg(msg);
    } catch (std::exception &e) {
      spdlog::error("Error on local insert:\nIndex-> {}\nMsg -> {}", index,
                    e.what());
    }

  } else {
    emit serverUnreachable();
  }

}

void Controller::onCharErased(int index) {

  if (state() == QTcpSocket::ConnectedState) {
    try {
      CrdtMessage msg(Type::ERASE, model->localErase(index),
                      model->getEditorId());
      sendMsg(msg);
    } catch (std::exception &e) {
      spdlog::error("Error on local erase: Index-> {} @ Msg -> {}", index,
                    e.what());
    }

  } else {
    emit serverUnreachable();
  }
}

void
Controller::onLoginRequest(const QString &username, const QString &password) {

  if (state() == QTcpSocket::ConnectedState) {
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(),
                                                         QCryptographicHash::Sha512);

    UserMessage msg(Type::LOGIN, model->getEditorId(),
                    User(username, QString(hashedPassword.toHex())));
    sendMsg(msg);

  } else {
    emit serverUnreachable();
  }
}

void Controller::onSignUpRequest(QString image, QString name, QString surname,
                                 QString username, QString email,
                                 const QString &password) {
  if (state() == QTcpSocket::ConnectedState) {
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(),
                                                         QCryptographicHash::Sha512);

    UserMessage msg(Type::REGISTER, model->getEditorId(),
                    User(std::move(image), std::move(username),
                         std::move(name), std::move(surname),
                         std::move(email),
                         QString(hashedPassword.toHex())));
    sendMsg(msg);
  } else {
    emit serverUnreachable();
  }
}

void Controller::onFileRequest(const QString &filename, bool exists) {

  if (state() == QTcpSocket::ConnectedState) {
    RequestMessage msg(exists ? Type::OPEN : Type::CREATE, model->getEditorId(),
                       filename);
    sendMsg(msg);
  } else {
    emit serverUnreachable();
  }

}
