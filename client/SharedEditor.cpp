//class SharedEditor;
// Created by s41m0n on 16/05/19.
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include "SharedEditor.h"
#include "../server/NetworkServer.h"
#include "../crdtAlgorithm/CrdtAlgorithm.h"

SharedEditor::SharedEditor(boost::asio::io_service& io_service,
const std::string& host, const std::string& service)
: conn(io_service), editorId(-1), _counter(0) {

    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host, service);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

    //Connect asynchronously
    conn.socket().async_connect(endpoint,
            boost::bind(&SharedEditor::handle_connect, this,
        boost::asio::placeholders::error, ++endpoint_iterator));
    spdlog::debug("SharedEditor?::Created");
}

