#include <QHostAddress>
#include <QCryptographicHash>
#include <QImage>
#include <memory>
#include <utility>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/UserMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "Controller.h"

Controller::Controller(Model *model, const std::string &host, int port)
        : model(model), socket(new TcpSocket(this)) {
  socket.connectToHost(QHostAddress(host.c_str()), port);
  connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void Controller::onReadyRead() {

  std::shared_ptr<BasicMessage> base(socket.readMsg());

  switch (base->getMsgType()) {
    case Type::CONNECT : {
      model->setEditorId(base->getEditorId());
      socket.setIdentifier(base->getEditorId());
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
              std::dynamic_pointer_cast<FileListingMessage>(base)->getFiles());
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
      base->getMsgType() == Type::INSERT ? model->remoteInsert(
              std::dynamic_pointer_cast<CrdtMessage>(base)->getSymbol())
                                         : model->remoteErase(
              std::dynamic_pointer_cast<CrdtMessage>(base)->getSymbol());
      emit remoteUpdate(model->textify());
      break;
    }
    default :
      throw std::runtime_error("Unknown message received");
  }
  if (socket.bytesAvailable()) {
    onReadyRead();
  }

}

void Controller::onCharInserted(int index, QChar value) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    auto symbol = model->localInsert(index, value);

    if (symbol != nullptr) {
      CrdtMessage msg(Type::INSERT, *symbol, model->getEditorId());
      socket.sendMsg(msg);
    }
  } else {
    emit serverUnreachable();
  }

}

void Controller::onCharErased(int index) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    auto symbol = model->localErase(index);

    if (symbol != nullptr) {
      CrdtMessage msg(Type::ERASE, *symbol, model->getEditorId());
      socket.sendMsg(msg);
    }
  } else {
    emit serverUnreachable();
  }
}

void
Controller::onLoginRequest(const QString &username, const QString &password) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(),
                                                         QCryptographicHash::Sha512);

    UserMessage msg(Type::LOGIN, model->getEditorId(),
                    User(username, QString(hashedPassword.toHex())));
    socket.sendMsg(msg);

  } else {
    emit serverUnreachable();
  }
}

void Controller::onSignUpRequest(QString image, QString name, QString surname,
                                 QString username, QString email,
                                 const QString &password) {
  if (socket.state() == QTcpSocket::ConnectedState) {
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(),
                                                         QCryptographicHash::Sha512);

    UserMessage msg(Type::REGISTER, model->getEditorId(),
                    User(std::move(image), std::move(username),
                         std::move(name), std::move(surname),
                         std::move(email),
                         QString(hashedPassword.toHex())));
    socket.sendMsg(msg);
  } else {
    emit serverUnreachable();
  }
}

void Controller::onFileRequest(const QString &filename, bool exists) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    RequestMessage msg(exists ? Type::OPEN : Type::CREATE, model->getEditorId(),
                       filename);
    socket.sendMsg(msg);
  } else {
    emit serverUnreachable();
  }

}
