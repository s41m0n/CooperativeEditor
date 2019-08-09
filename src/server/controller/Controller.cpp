//
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <fstream>
#include <boost/thread.hpp>
#include <src/utility/CrdtAlgorithm.h>
#include "Controller.h"

Controller::Controller(Model *model, unsigned short port) : model(model),
    acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
  spdlog::debug("NetworkServer::Created Controller");
}

Controller::~Controller() {
  spdlog::debug("NetworkServer::Destroyed Controller");
}

void Controller::handle_accept(const boost::system::error_code &e, connection_ptr &conn) {

  if (!e) {

    auto id = model->generateEditorId();

    std::lock_guard<std::mutex> guard(connectionsMutex);
    connections.insert(std::pair<int, connection_ptr>(id, conn));

    //Connected successfully, send editorId and start reading & listening recursively
    spdlog::debug("NetworkServer::Connected Shared Editor{}", id);
    auto msg = new BasicMessage(Type::CONNECT, id);
    conn->async_write(*msg,
                      boost::bind(&Controller::handle_write, this,
                                  boost::asio::placeholders::error, id, msg));

    //Start a thread to dispatch messages
    if (connections.size() == 2)
      boost::thread(boost::bind(&Controller::dispatch, this));

    // Start an accept operation for a new connection.
    connection_ptr new_conn(new Connection(acceptor_.get_io_service()));
    acceptor_.async_accept(new_conn->socket(),
                           boost::bind(&Controller::handle_accept, this,
                                       boost::asio::placeholders::error, new_conn));
  } else {
    spdlog::error("NetworkServer::Connect error -> {}", e.message());
  }
}

void Controller::handle_read(const boost::system::error_code &e, int connId, BasicMessage* msg) {

  if (!e) {

    spdlog::debug("NetworkServer::Received Message (type={}) from SharedEditor{}", msg->getMsgType(), connId);

    switch (msg->getMsgType()) {

      case Type::INSERT : {
        auto converted = dynamic_cast<CrdtMessage*>(msg);

        model->userInsert(connId, converted->getSymbol());

        std::lock_guard<std::mutex> guard2(queueMutex);
        messages.push(converted);

        auto newMsg = new CrdtMessage();
        connections[connId]->async_read(*newMsg,
                boost::bind(&Controller::handle_read, this,
                        boost::asio::placeholders::error, connId, newMsg));
        return;
      }
      case Type::ERASE : {
        auto converted = dynamic_cast<CrdtMessage*>(msg);

        model->userErase(connId, converted->getSymbol());

        std::lock_guard<std::mutex> guard2(queueMutex);
        messages.push(converted);

        auto newMsg = new CrdtMessage();
        connections[connId]->async_read(*newMsg,
                boost::bind(&Controller::handle_read, this,
                        boost::asio::placeholders::error, connId, newMsg));
        return;
      }
      case Type::CREATE : {
        std::string filename = dynamic_cast<RequestMessage*>(msg)->getFilename();

        if (model->createFileByUser(connId, filename)) {

          auto newMsg = new RequestMessage(Type::FILEOK, connId, filename);
          connections[connId]->async_write(*newMsg,
                                           boost::bind(&Controller::handle_write, this,
                                                       boost::asio::placeholders::error, connId, newMsg));
        } else {

          auto newMsg = new RequestMessage(Type::FILEKO, connId, filename);
          connections[connId]->async_write(*newMsg,
                                           boost::bind(&Controller::handle_write, this,
                                                       boost::asio::placeholders::error, connId, newMsg));
        }

        break;
      }
      case Type::OPEN : {
        std::string filename = dynamic_cast<RequestMessage*>(msg)->getFilename();

        if (model->openFileByUser(connId, filename)) {

          auto newMsg = new RequestMessage(Type::FILEOK, connId, filename);
          connections[connId]->async_write(*newMsg,
                  boost::bind(&Controller::handle_write, this,
                          boost::asio::placeholders::error, connId, newMsg));
        } else {

          auto newMsg = new RequestMessage(Type::FILEKO, connId, filename);
          connections[connId]->async_write(*newMsg,
                  boost::bind(&Controller::handle_write, this,
                          boost::asio::placeholders::error, connId, newMsg));
        }
        break;
      }
      default :
        throw std::runtime_error("NetworkServer::Should never read different types of Message");
    }
  } else {
    std::lock_guard<std::mutex> guard2(connectionsMutex);
    connections.erase(connId);
    spdlog::error("NetworkServer::Read error -> {}", e.message());
  }

  delete msg;
}

void Controller::handle_write(const boost::system::error_code &e, int connId, BasicMessage* msg) {

  if (!e) {

    spdlog::debug("NetworkServer::Sent Message (type={}) to SharedEditor{}", msg->getMsgType(), connId);

    switch (msg->getMsgType()) {

      case Type::CONNECT : {
        auto fileList = model->getAvailableFiles();
        auto newMsg = new FilesListingMessage(Type::LISTING, connId, fileList);
        connections[connId]->async_write(*newMsg,
                boost::bind(&Controller::handle_write, this,
                        boost::asio::placeholders::error, connId, newMsg));
        break;
      }
      case Type::LISTING : {
        auto newMsg = new RequestMessage();
        connections[connId]->async_read(*newMsg,
                boost::bind(&Controller::handle_read, this,
                        boost::asio::placeholders::error, connId, newMsg));
        break;
      }
      case Type::FILEOK : {
        auto symbolList = model->getFileSymbolList(connId);
        auto newMsg = new FileContentMessage(Type::CONTENT, connId, symbolList);
        connections[connId]->async_write(*newMsg,
                boost::bind(&Controller::handle_write, this,
                        boost::asio::placeholders::error, connId, newMsg));
        break;
      }
      case Type::FILEKO : {
        auto newMsg = new RequestMessage();
        connections[connId]->async_read(*newMsg,
                boost::bind(&Controller::handle_read, this,
                        boost::asio::placeholders::error, connId, newMsg));
        break;
      }
      case Type::CONTENT : {
        auto newMsg = new CrdtMessage();
        connections[connId]->async_read(*newMsg,
                boost::bind(&Controller::handle_read, this,
                        boost::asio::placeholders::error, connId, newMsg));
        break;
      }
      case Type::INSERT : return;
      case Type::ERASE : return;
      default: throw std::runtime_error("NetworkServer::Should never write other types of Message");
    }
  } else {
    std::lock_guard<std::mutex> guard2(connectionsMutex);
    connections.erase(connId);
    spdlog::error("NetworkServer::Write error -> {}", e.message());
  }

  delete msg;
}

void Controller::dispatch() {

  while (connections.size() >= 2) {

    std::lock_guard<std::mutex> guard1(queueMutex);

    for (int i = 0, size = this->messages.size(); i < size; i++) {

      auto msg = this->messages.front();
      std::lock_guard<std::mutex> guard2(connectionsMutex);

      for (auto& conn : connections) {

        if (conn.first != msg->getEditorId())
          conn.second->async_write(*msg,
                  boost::bind(&Controller::handle_write, this,
                          boost::asio::placeholders::error, conn.first, msg));
      }

      this->messages.pop();
      delete msg;
    }
  }
}


int Controller::start() {
  //Create new connection and start listening asynchronously
  connection_ptr new_conn(new Connection(acceptor_.get_io_service()));
  acceptor_.async_accept(new_conn->socket(),
                         boost::bind(&Controller::handle_accept, this,
                                     boost::asio::placeholders::error, new_conn));
  return io_service.run();
}