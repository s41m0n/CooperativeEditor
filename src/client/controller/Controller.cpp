//class SharedEditor;
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <QHostAddress>
#include <QAbstractSocket>

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
    case Type::FILEOK : {
      RequestMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      break;
    }
    case Type::FILEKO : {
      RequestMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      throw std::runtime_error("Unable to create file");
    }
    case Type::CONTENT : {
      FileContentMessage msg(std::move(base));
      ds >> msg;
      spdlog::debug("Received Message!\n{}", msg.toString());
      model->setCurrentFileContent(msg.getSymbols());
      std::vector<std::pair<int, char>> toInsert({{0, 's'},
                                                  {1, 'i'},
                                                  {2, 'm'},
                                                  {3, 'o'}});
      std::vector<int> toDelete({});

      for (auto pair : toInsert) {
        handle_insert(pair.first, pair.second);
      }

      for (auto pos : toDelete) {
        handle_erase(pos);
      }

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
