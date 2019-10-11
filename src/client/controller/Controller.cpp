#include <spdlog/spdlog.h>
#include <QHostAddress>
#include <QCryptographicHash>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/LoginMessage.h"
#include "src/components/messages/RequestMessage.h"
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

/// Handle completion of a read operation.
void Controller::onReadyRead() {

  auto base = socket.readMsg();

  switch (base->getMsgType()) {
    case Type::CONNECT : {
      spdlog::debug("Received Message!\n{}", base->toString());
      model->setEditorId(base->getEditorId());
      break;
    }
    case Type::LOGIN_RESULT : {
      auto derived = dynamic_cast<ResultMessage*>(base);
      spdlog::debug("Received Message!\n{}", derived->toString());
      emit loginResponse(derived->isPositive());
      break;
    }
    case Type::LISTING : {
      auto derived = dynamic_cast<FileListingMessage*>(base);
      spdlog::debug("Received Message!\n{}", derived->toString());

      emit fileListing(derived->getFiles());
      break;
    }
    case Type::FILE_RESULT : {
      auto derived = dynamic_cast<ResultMessage*>(base);
      spdlog::debug("Received Message!\n{}", derived->toString());
      if (!derived->isPositive()) {
        emit fileResult(false);
      }
      break;
    }
    case Type::CONTENT : {
      auto derived = dynamic_cast<FileContentMessage*>(base);
      spdlog::debug("Received Message!\n{}", derived->toString());
      model->setCurrentFileContent(derived->getSymbols());
      emit fileResult(true);
      break;
    }
    case Type::INSERT : {
      auto derived = dynamic_cast<CrdtMessage*>(base);
      spdlog::debug("Received Message!\n{}", derived->toString());
      model->remoteInsert(derived->getSymbol());
      emit remoteUpdate(model->textify());
      break;
    }
    case Type::ERASE : {
      auto derived = dynamic_cast<CrdtMessage*>(base);
      spdlog::debug("Received Message!\n{}", base->toString());
      model->remoteErase(derived->getSymbol());
      break;
    }
    default :
      throw std::runtime_error("Unknown message received");
  }
  delete base;
  if (socket.bytesAvailable()) {
    onReadyRead();
  }

}

void Controller::onCharInserted(int index, char value) {

  auto symbol = model->localInsert(index, value);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::INSERT, *symbol, model->getEditorId());
    socket.sendMsg(msg);
    spdlog::debug("Inserted Symbol!\n{}", symbol->toString());
    spdlog::debug("Current text: {}", model->textify());
  }
}

void Controller::onCharErased(int index) {

  auto symbol = model->localErase(index);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::ERASE, *symbol, model->getEditorId());
    socket.sendMsg(msg);
    spdlog::debug("Erased Symbol!\n{}", symbol->toString());
    spdlog::debug("Current text: {}", model->textify());
  }
}

void
Controller::onLoginRequest(const QString &username, const QString &password) {

  if (socket.state() == QTcpSocket::ConnectedState) {
    QByteArray hashedPassword = QCryptographicHash::hash(password.toUtf8(),
                                                         QCryptographicHash::Sha512);

    LoginMessage msg(model->getEditorId(), username.toStdString(),
                     QString(hashedPassword.toHex()).toStdString());
    socket.sendMsg(msg);
    spdlog::debug("Login Request sent!\n{}", msg.toString());

  } else {
    emit serverUnreachable();
  }

}
