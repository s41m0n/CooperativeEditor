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

