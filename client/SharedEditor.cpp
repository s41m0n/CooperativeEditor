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


SharedEditor::~SharedEditor() {
    spdlog::debug("SharedEditor{0:d}::Destroyed", editorId);
}

/// Handle completion of a connect operation.
void SharedEditor::handle_connect(const boost::system::error_code& e,
                    boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
    if (!e) {

        //Successfully connect, start reading (recursively)
        std::shared_ptr<Message> incomingMsg(new Message());
        conn.async_read(*incomingMsg,
                               boost::bind(&SharedEditor::handle_read, this,
                                           boost::asio::placeholders::error, incomingMsg));
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator()) {
        // Try to reconnect to next endpoint
        conn.socket().close();
        boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
        conn.socket().async_connect(endpoint,
                                           boost::bind(&SharedEditor::handle_connect, this,
                                                       boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else {

        //Error while connecting
        spdlog::error("SharedEditor::Connect error -> {}", e.message());
    }
}

/// Handle completion of a read operation.
void SharedEditor::handle_read(const boost::system::error_code& e, std::shared_ptr<Message>& incomingMsg) {

    if (!e) {

        //Successfully read, process and prepare to read again
        this->process(incomingMsg);

        std::shared_ptr<Message> newMsg(new Message());
        conn.async_read(*newMsg,
                        boost::bind(&SharedEditor::handle_read, this,
                                    boost::asio::placeholders::error, newMsg));
    } else {
        //Error while reading (may be also socket shutdown)
        spdlog::error("SharedEditor::Read error -> {}", e.message());
    }
}

void SharedEditor::handle_write(const boost::system::error_code& e, std::shared_ptr<Message>& msg)
{
    if(!e)
        spdlog::debug("SharedEditor{}::Sent Message (type={})", editorId, msg->getMsgType());
    else
        // Error while writing (same).
        spdlog::error("SharedEditor::Write error -> {}", e.message());
}

