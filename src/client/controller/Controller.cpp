#include <spdlog/spdlog.h>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QCryptographicHash>

#include "components/Message.h"
#include "client/controller/Controller.h"

Controller::Controller(Model *model, const std::string &host, int port)
        : model(model), _socket(this) {
  _socket.connectToHost(QHostAddress(host.c_str()), port);
  connect(&_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  spdlog::debug("Created Controller");
}

Controller::~Controller() {
  spdlog::debug("Destroyed Controller");
}

/// Handle completion of a read operation.
void Controller::onReadyRead() {
  QDataStream ds(&_socket);
  BasicMessage base;
  ds >> base;

  switch (base.getMsgType()) {
    case Type::CONNECT : {
      spdlog::debug("Received Message!\n{}", base.toString());
      model->setEditorId(base.getEditorId());
      break;
    }
    case Type::LOGIN_RESULT : {
      ResultMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      if (!msg.isPositive()) {
        throw std::runtime_error("Login Failed");
      }
      break;
    }
    case Type::LISTING : {
      RequestMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());

      auto availableFiles = msg.getFilename();
      std::string fileToOpen;
      Type type;

      if (availableFiles.empty()) {
        fileToOpen = "prova.txt";
        type = Type::CREATE;
      } else {
        fileToOpen = availableFiles.substr(0, availableFiles.find_last_of(';'));
        type = Type::OPEN;
      }
      model->setCurrentFile(fileToOpen);
      RequestMessage msgToSend(type, model->getEditorId(), fileToOpen);
      ds << msgToSend;
      break;
    }
    case Type::FILE_RESULT : {
      ResultMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      if (!msg.isPositive()) {
        throw std::runtime_error("Unable to create file");
      }
      break;
    }
    case Type::CONTENT : {
      FileContentMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->setCurrentFileContent(msg.getSymbols());
      break;
    }
    case Type::INSERT : {
      CrdtMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->remoteInsert(msg.getSymbol());
      break;
    }
    case Type::ERASE : {
      CrdtMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->remoteErase(msg.getSymbol());
      break;
    }
    default :
      throw std::runtime_error("Unknown message received");
  }

  if (_socket.bytesAvailable()) {
    onReadyRead();
  }

}

void Controller::handle_insert(int index, char value) {

  auto symbol = model->localInsert(index, value);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::INSERT, *symbol, model->getEditorId());
    QDataStream ds(&_socket);
    ds << msg;
    spdlog::debug("Inserted Symbol!\n{}", symbol->toString());
    spdlog::debug("Current text: {}", model->textify());
  }
}

void Controller::handle_erase(int index) {

  auto symbol = model->localErase(index);

  if (symbol != nullptr) {
    CrdtMessage msg(Type::ERASE, *symbol, model->getEditorId());
    QDataStream ds(&_socket);
    ds << msg;
    spdlog::debug("Erased Symbol!\n{}", symbol->toString());
    spdlog::debug("Current text: {}", model->textify());
  }
}

void
Controller::onLoginRequest(std::string username, const std::string &password) {

  QByteArray hashedPassword = QCryptographicHash::hash(
          QByteArray::fromStdString(password), QCryptographicHash::Sha3_512);

  auto stringHashedPassword = hashedPassword.toStdString(); //discuti con Simo su algoritmo da usare e se possono dare problemi caratteri dati da .toStdString

  LoginMessage msg(model->getEditorId(), username, stringHashedPassword);
  QDataStream ds(&_socket);
  ds << msg;
  spdlog::debug("Login Request sent!\n{}", msg.toString());

}
