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

NetworkServer::~NetworkServer() {
    spdlog::debug("NetworkServer::Destroyed");
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

void NetworkServer::handle_read(const boost::system::error_code& e, int connId, std::shared_ptr<Message>& msg) {
    if (!e) {

        //If still present, read recursively
        std::shared_ptr<Message> newMsg(new Message());
        if(this->connections.find(connId) != this->connections.end())
            this->connections[connId]->async_read(*newMsg,
                    boost::bind(&NetworkServer::handle_read, this,
                            boost::asio::placeholders::error, connId, newMsg));

        {
            //Update symbols array
            std::lock_guard<std::mutex> guard(this->symbolsMutes);
            switch(msg->getMsgType()) {
                case INSERT:
                    this->remoteInsert(msg->getSymbol());
                    break;
                case ERASE:
                    this->remoteErase(msg->getSymbol());
                    break;
                default:
                    throw std::runtime_error("NetworkServer::Received bad message in function handle");
            }
        }

        {
            //Read successfully, insert Message in the queue
            spdlog::debug("NetworkServer::Received Message (type={}, editorId={}) from SharedEditor{}", msg->getMsgType(), msg->getEditorId(), connId);
            std::lock_guard<std::mutex> guard(this->queueMutex);
            messages.push(std::move(msg));
        }

    } else {
        // Error while reading (socket down?)
        std::lock_guard<std::mutex> guard(this->connectionMapMutex);
        if(!this->connections.empty() && this->connections.find(connId) != this->connections.end())
            this->connections.erase(connId);
        spdlog::error("NetworkServer::Read error -> {}", e.message());
    }
}

void NetworkServer::handle_write(const boost::system::error_code& e, int connId, std::shared_ptr<Message>& msg)
{

    if(!e) {

        //Write successfully
        spdlog::debug("NetworkServer::Sent Message (type={}, editorId={}) to SharedEditor{}", msg->getMsgType(), msg->getEditorId(), connId);

    }else {

        //Error, write failed (socket shutdown?)
        std::lock_guard<std::mutex> guard(this->connectionMapMutex);
        if(!this->connections.empty() && this->connections.find(connId) != this->connections.end())
            this->connections.erase(connId);
        spdlog::error("NetworkServer::Write error -> {}", e.message());
    }
}

void NetworkServer::dispatch() {
    std::lock_guard<std::mutex> guard1(this->queueMutex);
    for(int i=0, size=this->messages.size(); i<size; i++) {
        auto m = this->messages.front();
        std::lock_guard<std::mutex> guard2(this->connectionMapMutex);
        for(std::pair<int, connection_ptr> pair : connections) {
            if(pair.first != m->getEditorId() && pair.second->socket().is_open())
                pair.second->async_write(*m,
                                         boost::bind(&NetworkServer::handle_write, this,
                                                     boost::asio::placeholders::error, pair.first, m));
        }
        this->messages.pop();
    }
}

void NetworkServer::remoteErase(Symbol s) {
    int index = CrdtAlgorithm::findPositionErase(std::move(s), this->symbols);
    if(index >= 0) {
        this->symbols.erase(this->symbols.begin() + index);
        this->writeOnFile();
    }
}

void NetworkServer::remoteInsert(Symbol s) {
    int index = CrdtAlgorithm::findPositionInsert(s, this->symbols);

    this->symbols.insert(this->symbols.begin()+index, std::move(s));
    this->writeOnFile();
}

void NetworkServer::writeOnFile() {
    std::ofstream outfile;
    std::string filename = "../../testNS.txt";
    outfile.open(filename, std::ios_base::app);
    outfile << this->to_string() << std::endl;
}

std::string NetworkServer::to_string() {
    std::string str;
    std::for_each(this->symbols.begin(), this->symbols.end(), [&str](Symbol s){
        str += s.getChar();
    });
    return str;
}
