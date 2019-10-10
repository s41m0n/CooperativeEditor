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

  BasicMessage base;
  socket >> base;

  switch (base.getMsgType()) {
    case Type::CONNECT : {
      spdlog::debug("Received Message!\n{}", base.toString());
      model->setEditorId(base.getEditorId());
      break;
    }
    case Type::LOGIN_RESULT : {
      ResultMessage msg(std::move(base));
      socket >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      emit loginResponse(msg.isPositive());
      break;
    }
    case Type::LISTING : {
      FileListingMessage msg(std::move(base));
      socket >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());

      emit fileListing(msg.getFiles());
      break;
    }
    case Type::FILE_RESULT : {
      ResultMessage msg(std::move(base));
      socket >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      if (!msg.isPositive()) {
        emit fileResult(false);
      }
      break;
    }
    case Type::CONTENT : {
      FileContentMessage msg(std::move(base));
      socket >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->setCurrentFileContent(msg.getSymbols());
      emit fileResult(true);
      break;
    }
    case Type::INSERT : {
      CrdtMessage msg(std::move(base));
      socket >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->remoteInsert(msg.getSymbol());
      emit remoteUpdate(model->textify());
      break;
    }
    case Type::ERASE : {
      CrdtMessage msg(std::move(base));
      socket >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->remoteErase(msg.getSymbol());
      break;
    }
    default :
      throw std::runtime_error("Unknown message received");
  }

  if (socket.bytesAvailable()) {
    onReadyRead();
  }

}

void Controller::onCharInserted(int index, char value) {

  auto symbol = model->localInsert(index, value);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::INSERT, *symbol, model->getEditorId());
    socket << msg;
    spdlog::debug("Inserted Symbol!\n{}", symbol->toString());
    spdlog::debug("Current text: {}", model->textify());
  }
}

void Controller::onCharErased(int index) {

  auto symbol = model->localErase(index);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::ERASE, *symbol, model->getEditorId());
    socket << msg;
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

    socket << msg;
    spdlog::debug("Login Request sent!\n{}", msg.toString());

  } else {
    emit serverUnreachable();
  }

}
