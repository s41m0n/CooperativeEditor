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

    auto header = getHeader();
    std::shared_ptr<BasicMessage> base(readMsg());

    switch (header.getType()) {
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
          getHeader().getType() == Type::INSERT ? model->remoteInsert(
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
  if (isMessageAvailable()) {
    onReadyRead();
  }

}

void Controller::onCharInserted(int index, QChar value) {

  if (state() == QTcpSocket::ConnectedState) {
    try {
      CrdtMessage msg(model->localInsert(index, value),
                      model->getEditorId());
      sendMsg(Type::INSERT, msg);
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
      CrdtMessage msg(model->localErase(index),
                      model->getEditorId());
      sendMsg(Type::ERASE, msg);
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

    UserMessage msg(model->getEditorId(),
                    User(username, QString(hashedPassword.toHex())));
    sendMsg(Type::LOGIN, msg);

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

    UserMessage msg(model->getEditorId(),
                    User(std::move(image), std::move(username),
                         std::move(name), std::move(surname),
                         std::move(email),
                         QString(hashedPassword.toHex())));
    sendMsg(Type::REGISTER, msg);
  } else {
    emit serverUnreachable();
  }
}

void Controller::onFileRequest(const QString &filename, bool exists) {

  if (state() == QTcpSocket::ConnectedState) {
    RequestMessage msg(model->getEditorId(), filename);
    sendMsg(exists ? Type::OPEN : Type::CREATE, msg);
  } else {
    emit serverUnreachable();
  }

}
