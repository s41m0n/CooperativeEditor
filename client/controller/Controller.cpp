//class SharedEditor;
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <boost/thread.hpp>
#include <fstream>
#include "Controller.h"

Controller::Controller(const std::string& host, const std::string& service)
: model(new Model()), view(), conn(io_service) {

  // Resolve the host name into an IP address.
  boost::asio::ip::tcp::resolver resolver(io_service);
  boost::asio::ip::tcp::resolver::query query(host, service);
  boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
  boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

  //Connect asynchronously
  conn.socket().async_connect(endpoint,
                              boost::bind(&Controller::handle_connect, this,
                                          boost::asio::placeholders::error, ++endpoint_iterator));
  spdlog::debug("SharedEditor::Created Controller");
}


Controller::~Controller() {
  spdlog::debug("SharedEditor{0:d}::Destroyed Controller", model->getEditorId());
}

/// Handle completion of a connect operation.
void Controller::handle_connect(const boost::system::error_code& e,
                    boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
    if (!e) {

      //Successfully connect, receive editor ID
      auto msg = new BasicMessage();
      conn.async_read(*msg,
                      boost::bind(&Controller::handle_read, this,
                                  boost::asio::placeholders::error, msg));


    } else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
      // Try to reconnect to next endpoint
      conn.socket().close();
      boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
      conn.socket().async_connect(endpoint,
                                  boost::bind(&Controller::handle_connect, this,
                                              boost::asio::placeholders::error, ++endpoint_iterator));
    } else {
      //Error while connecting
      spdlog::error("SharedEditor::Connect error -> {}", e.message());
    }
}

/// Handle completion of a read operation.
void Controller::handle_read(const boost::system::error_code& e, BasicMessage* msg) {

    if (!e) {
      spdlog::debug("SharedEditor{}::Received Message (type={})", model->getEditorId(), msg->getMsgType());

      switch (msg->getMsgType()) {

        case Type::CONNECT: {
          model->setEditorId(msg->getEditorId());
          auto newMsg = new FilesListingMessage();
          conn.async_read(*newMsg,
                          boost::bind(&Controller::handle_read, this,
                                      boost::asio::placeholders::error, newMsg));
          break;
        }
        case Type::LISTING : {
          std::string availableFiles = dynamic_cast<FilesListingMessage *>(msg)->getFiles();
          std::string newFile;
          Type type;

          if (availableFiles.empty()) {
            newFile = "prova.txt";
            type = Type::CREATE;
          } else {
            newFile = availableFiles.substr(0, availableFiles.find_last_of(';'));
            type = Type::OPEN;
          }

          model->setCurrentFile(newFile);
          auto newMsg = new RequestMessage(type, model->getEditorId(), newFile);
          conn.async_write(*newMsg,
                           boost::bind(&Controller::handle_write, this,
                                       boost::asio::placeholders::error, newMsg));
          break;
        }
        case Type::FILEOK: {
          auto newMsg = new FileContentMessage();
          conn.async_read(*newMsg,
                          boost::bind(&Controller::handle_read, this,
                                      boost::asio::placeholders::error, newMsg));
          break;
        }
        case Type::CONTENT: {
          model->setCurrentFileContent(dynamic_cast<FileContentMessage *>(msg)->getSymbols());
          spdlog::debug("Received content -> {}", model->to_string());
          ready = true;
          break;
        }
        case Type::FILEKO : {
          throw std::runtime_error("SharedEditor::Unable to create file");
        }
        case Type::INSERT : {
          model->remoteInsert(dynamic_cast<CrdtMessage *>(msg)->getSymbol());
          auto newMsg = new CrdtMessage();
          conn.async_read(*newMsg,
                          boost::bind(&Controller::handle_read, this,
                                      boost::asio::placeholders::error, newMsg));
          break;
        }
        case Type::ERASE : {
          model->remoteErase(dynamic_cast<CrdtMessage *>(msg)->getSymbol());
          auto newMsg = new CrdtMessage();
          conn.async_read(*newMsg,
                          boost::bind(&Controller::handle_read, this,
                                      boost::asio::placeholders::error, newMsg));
          break;
        }
        default:
          throw std::runtime_error("SharedEditor::Bad message received from server");
      }

    }
    else {
      //Error while reading (may be also socket shutdown)
      spdlog::error("SharedEditor::Read error -> {}", e.message());
    }
    delete msg;
}

void Controller::handle_write(const boost::system::error_code& e, BasicMessage* msg)
{
    if(!e) {

      spdlog::debug("SharedEditor{}::Sent Message (type={})", model->getEditorId(), msg->getMsgType());

      if (msg->getMsgType() == Type::CREATE || msg->getMsgType() == Type::OPEN) {
        auto newMsg = new RequestMessage();
        conn.async_read(*newMsg,
                        boost::bind(&Controller::handle_read, this,
                                    boost::asio::placeholders::error, newMsg));
      }
    } else {
      // Error while writing (same).
      spdlog::error("SharedEditor::Write error -> {}", e.message());
    }
    delete msg;
}

void Controller::handle_insert(int index, char value) {

  while(!ready);

  const Symbol* symbol = model->localInsert(index, value);

  if(symbol != nullptr) {
    auto newMsg = new CrdtMessage(Type::INSERT, *symbol, model->getEditorId());
    conn.async_write(*newMsg,
                     boost::bind(&Controller::handle_write, this,
                                 boost::asio::placeholders::error, newMsg));
  }
}

void Controller::handle_erase(int index) {
  while(!ready);

  const Symbol *symbol = model->localErase(index);

  if(symbol != nullptr) {
    auto newMsg = new CrdtMessage(Type::ERASE, *symbol, model->getEditorId());
    conn.async_write(*newMsg,
                     boost::bind(&Controller::handle_write, this,
                                 boost::asio::placeholders::error, newMsg));
  }
}

void Controller::setView(View* newView){
  view = newView;
}

int Controller::start() {
  return io_service.run();
}