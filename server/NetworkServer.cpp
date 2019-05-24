//
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <fstream>
#include "NetworkServer.h"
#include "../network/Connection.h"
#include "../crdtAlgorithm/CrdtAlgorithm.h"

NetworkServer::NetworkServer(boost::asio::io_service& io_service, unsigned short port)
: acceptor_(io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), symbols() {

    spdlog::debug("NetworkServer::Created NetworkServer");

    //Create new connection and start listening asynchronously
    connection_ptr new_conn(new Connection(acceptor_.get_io_service()));
    acceptor_.async_accept(new_conn->socket(),
        boost::bind(&NetworkServer::handle_accept, this,
                    boost::asio::placeholders::error, new_conn));
}

void NetworkServer::handle_accept(const boost::system::error_code& e, connection_ptr& conn)
{
    if (!e) {

        //Data read successfully, send editorId and start reading & listening recursively
        spdlog::debug("NetworkServer::Connected Shared Editor n. {0:d}", idGenerator);
        std::shared_ptr<Message> msg(new Message(CONNECT, idGenerator));
        conn->async_write(*msg,
                          boost::bind(&NetworkServer::handle_write, this,
                                      boost::asio::placeholders::error, idGenerator, msg));
        {
            std::lock_guard<std::mutex> guard(this->connectionMapMutex);
            if (connections.find(idGenerator) == connections.end())
                connections.insert(std::pair<int, connection_ptr>(idGenerator, conn));
        }

        {
            std::lock_guard<std::mutex> guard(this->symbolsMutes);
            for(Symbol& s : this->symbols) {
                std::shared_ptr<Message> newMsg(new Message(INSERT, s, idGenerator));
                conn->async_write(*newMsg,
                                  boost::bind(&NetworkServer::handle_write, this,
                                              boost::asio::placeholders::error, idGenerator, newMsg));
            }
        }

        std::shared_ptr<Message> newMsg(new Message());
        conn->async_read(*newMsg,
                        boost::bind(&NetworkServer::handle_read, this,
                                    boost::asio::placeholders::error, idGenerator, newMsg));

        idGenerator++;

        // Start an accept operation for a new connection.
        connection_ptr new_conn(new Connection(acceptor_.get_io_service()));
        acceptor_.async_accept(new_conn->socket(),
                               boost::bind(&NetworkServer::handle_accept, this,
                                           boost::asio::placeholders::error, new_conn));
    }
    else {
        spdlog::error("NetworkServer::Connect error -> {}", e.message());
    }
}