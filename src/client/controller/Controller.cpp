#include <QHostAddress>
#include <QCryptographicHash>
#include <QImage>
#include <memory>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/LoginMessage.h"
#include "src/components/messages/FileContentMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/ResultMessage.h"
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
      break;
    }
    case Type::LOGIN_RESULT : {
      emit loginResponse(
              std::dynamic_pointer_cast<ResultMessage>(base)->isPositive());
      break;
    }
    case Type::LISTING : {
      emit fileListing(
              std::dynamic_pointer_cast<FileListingMessage>(base)->getFiles());
      break;
    }
    case Type::FILE_RESULT : {
      emit fileResult(
              std::dynamic_pointer_cast<ResultMessage>(base)->isPositive());
      break;
    }
    case Type::CONTENT : {
      model->setCurrentFileContent(
              std::dynamic_pointer_cast<FileContentMessage>(
                      base)->getSymbols());
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

  auto symbol = model->localInsert(index, value);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::INSERT, *symbol, model->getEditorId());
    socket.sendMsg(msg);
  }
}

void Controller::onCharErased(int index) {

  auto symbol = model->localErase(index);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::ERASE, *symbol, model->getEditorId());
    socket.sendMsg(msg);
  }
}

void
Controller::onLoginRequest(const QString &username, const QString &password) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(),
                                                         QCryptographicHash::Sha512);

    LoginMessage msg(model->getEditorId(), username,
                     QString(hashedPassword.toHex()));
    socket.sendMsg(msg);

  } else {
    emit serverUnreachable();
  }

}

void Controller::onSignUpRequest(QString image, QString name, QString surname,
                                 QString username, QString email,
                                 QString password) {
  //TODO : send data to server (Refactor to do using a User class)

}

void Controller::onFileRequest(const QString &filename, bool exists) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    model->setCurrentFile(filename);
    RequestMessage msg(exists ? Type::OPEN : Type::CREATE, model->getEditorId(),
                       filename);
    socket.sendMsg(msg);
  } else {
    emit serverUnreachable();
  }

}
