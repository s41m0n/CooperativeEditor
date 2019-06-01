//
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <fstream>
#include <boost/thread.hpp>
#include "NetworkServer.h"

NetworkServer::NetworkServer(boost::asio::io_service& io_service, unsigned short port)
: acceptor_(io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)), openedFiles(), idGenerator(0) {

    spdlog::debug("NetworkServer::Created NetworkServer");

    loadAllFileNames();

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

        connections.insert(std::pair<int, std::pair<std::string, connection_ptr>>(idGenerator,
                std::pair<std::string, connection_ptr>("",conn)));

        buffers.insert(std::pair<int, std::pair<boost::shared_ptr<BasicMessage>, boost::shared_ptr<BasicMessage>>>(
                idGenerator, std::pair<boost::shared_ptr<BasicMessage>, boost::shared_ptr<BasicMessage>>()));

        //Connected successfully, send editorId and start reading & listening recursively
        spdlog::debug("NetworkServer::Connected Shared Editor{}", idGenerator);
        buffers[idGenerator].second = boost::make_shared<BasicMessage>(Type::CONNECT, idGenerator);
        conn->async_write(buffers[idGenerator].second,
                          boost::bind(&NetworkServer::handle_write, this,
                                      boost::asio::placeholders::error, idGenerator));

        //Start a thread to dispatch messages
        if(connections.size() == 2)
            boost::thread(boost::bind(&NetworkServer::dispatch, this));


        idGenerator++;

        // Start an accept operation for a new connection.
        connection_ptr new_conn(new Connection(acceptor_.get_io_service()));
        acceptor_.async_accept(new_conn->socket(),
                               boost::bind(&NetworkServer::handle_accept, this,
                                           boost::asio::placeholders::error, new_conn));
    }
    else
        spdlog::error("NetworkServer::Connect error -> {}", e.message());

}

void NetworkServer::handle_read(const boost::system::error_code& e, int connId) {

    if (!e) {
        spdlog::debug("NetworkServer::Received Message (type={}) from SharedEditor{}", buffers[connId].first->getMsgType(), connId);

        switch (buffers[connId].first->getMsgType()) {

            case Type::INSERT : {
                boost::shared_ptr<CrdtMessage> converted = boost::static_pointer_cast<CrdtMessage>(buffers[connId].first);
                std::string filename = connections[connId].first;

                std::lock_guard<std::mutex> guard(symbolsMutes);
                CrdtAlgorithm::remoteInsert(converted->getSymbol(), openedFiles[filename]);

                writeOnFile(filename);

                std::lock_guard<std::mutex> guard2(queueMutex);
                messages.push(std::move(converted));
                break;
            }
            case Type::ERASE : {
                boost::shared_ptr<CrdtMessage> converted = boost::static_pointer_cast<CrdtMessage>(buffers[connId].first);
                std::string filename = connections[connId].first;

                std::lock_guard<std::mutex> guard(symbolsMutes);
                CrdtAlgorithm::remoteErase(converted->getSymbol(), openedFiles[filename]);

                writeOnFile(filename);

                std::lock_guard<std::mutex> guard2(queueMutex);
                messages.push(std::move(converted));
                break;
            }
            case Type::CREATE : {
                std::string filename = boost::static_pointer_cast<RequestMessage>(buffers[connId].first)->getFilename();

                if(availableFiles.find(filename) != std::string::npos) {

                    buffers[connId].second = boost::make_shared<RequestMessage>(Type::FILEKO, connId, filename);
                    connections[connId].second->async_write(buffers[connId].second,
                            boost::bind(&NetworkServer::handle_write, this,
                                    boost::asio::placeholders::error, connId));
                } else {

                    connections[connId].first = filename;
                    availableFiles.append(filename + ";");

                    openedFiles.insert(std::pair<std::string, std::vector<Symbol>>(filename, std::vector<Symbol>()));

                    buffers[connId].second = boost::make_shared<RequestMessage>(Type::FILEOK, connId, connections[connId].first);
                    connections[connId].second->async_write(buffers[connId].second,
                            boost::bind(&NetworkServer::handle_write, this,
                                    boost::asio::placeholders::error, connId));
                }

                return;
            }
            case Type::OPEN : {
                std::string filename = boost::static_pointer_cast<RequestMessage>(buffers[connId].first)->getFilename();

                if(openedFiles.empty() || openedFiles.find(filename) == openedFiles.end()) {

                    openedFiles.insert(
                            std::pair<std::string, std::vector<Symbol>>(filename, std::vector<Symbol>()));

                    restoreFromFile(filename);

                    connections[connId].first = filename;
                    buffers[connId].second = boost::make_shared<RequestMessage>(Type::FILEOK, connId, filename);
                    connections[connId].second->async_write(buffers[connId].second,
                            boost::bind(&NetworkServer::handle_write, this,
                                    boost::asio::placeholders::error, connId));
                } else {

                    buffers[connId].second = boost::make_shared<RequestMessage>(Type::FILEKO, connId, filename);
                    connections[connId].second->async_write(buffers[connId].second,
                            boost::bind(&NetworkServer::handle_write, this,
                                    boost::asio::placeholders::error, connId));
                }
                return;
            }
            default :
                throw std::runtime_error("NetworkServer::Should never read different types of Message");

        }

        buffers[connId].first = boost::make_shared<CrdtMessage>();
        connections[connId].second->async_read(buffers[connId].first,
                boost::bind(&NetworkServer::handle_read, this,
                        boost::asio::placeholders::error, connId));
    } else {

        buffers.erase(connId);
        connections.erase(connId);
        spdlog::error("NetworkServer::Read error -> {}", e.message());
    }
}

void NetworkServer::handle_write(const boost::system::error_code& e, int connId)
{

    if(!e) {

        spdlog::debug("NetworkServer::Sent Message (type={}) to SharedEditor{}", buffers[connId].second->getMsgType(), connId);
        //Write successfully
        switch (buffers[connId].second->getMsgType()) {

            case Type::CONNECT : {
                buffers[connId].second = boost::make_shared<FilesListingMessage>(Type::LISTING, connId, availableFiles);
                connections[connId].second->async_write(buffers[connId].second,
                        boost::bind(&NetworkServer::handle_write, this,
                                boost::asio::placeholders::error, connId));
                break;
            }
            case Type::LISTING : {
                buffers[connId].first = boost::make_shared<RequestMessage>();
                connections[connId].second->async_read(buffers[connId].first,
                        boost::bind(&NetworkServer::handle_read, this,
                                boost::asio::placeholders::error, connId));
                break;
            }
            case Type::FILEOK : {
                buffers[connId].second = boost::make_shared<FileContentMessage>(Type::CONTENT, connId, openedFiles[connections[connId].first]);
                connections[connId].second->async_write(buffers[connId].second ,
                        boost::bind(&NetworkServer::handle_write, this,
                                boost::asio::placeholders::error, connId));
                break;
            }
            case Type::FILEKO : {
                buffers[connId].first = boost::make_shared<RequestMessage>();
                connections[connId].second->async_read(buffers[connId].first,
                        boost::bind(&NetworkServer::handle_read, this,
                                boost::asio::placeholders::error, connId));
                break;
            }
            case Type::CONTENT : {
                buffers[connId].first = boost::make_shared<CrdtMessage>();
                connections[connId].second->async_read(buffers[connId].first,
                        boost::bind(&NetworkServer::handle_read, this,
                                boost::asio::placeholders::error, connId));

                break;
            }
            default:
                throw std::runtime_error("NetworkServer::Should never write other types of Message");
        }
    } else {

        buffers.erase(connId);
        connections.erase(connId);
        spdlog::error("NetworkServer::Write error -> {}", e.message());
    }

}

void NetworkServer::dispatch() {

    while(connections.size() >= 2) {

        std::lock_guard<std::mutex> guard1(this->queueMutex);

        for(int i=0, size=this->messages.size(); i<size; i++) {

            auto msg = this->messages.front();
            std::lock_guard<std::mutex> guard2(this->connectionMapMutex);

            for(std::pair<int, std::pair<std::string,connection_ptr>> pair : connections) {

                if(pair.first != msg->getEditorId() && pair.second.second->socket().is_open())
                    pair.second.second->async_write(msg,
                                                    boost::bind(&NetworkServer::handle_write, this,
                                                                boost::asio::placeholders::error, pair.first));
            }

            this->messages.pop();
        }
    }
}

void NetworkServer::writeOnFile(std::string& filename) {
    std::ofstream file{filename + ".crdt"};
    boost::archive::text_oarchive oa{file};
    oa << openedFiles[filename];
}

void NetworkServer::restoreFromFile(std::string& filename) {
    std::ifstream file{filename + ".crdt"};
    boost::archive::text_iarchive oa{file};
    oa >> openedFiles[filename];
}

void NetworkServer::loadAllFileNames() {

    boost::filesystem::recursive_directory_iterator it(boost::filesystem::current_path());
    boost::filesystem::recursive_directory_iterator end;

    while(it != end) {

        if(boost::filesystem::is_regular_file(*it) && it->path().extension() == ".crdt") {
            std::string toAdd = it->path().filename().string();
            size_t index = toAdd.find_last_of('.');
            toAdd = toAdd.substr(0, index);
            availableFiles.append(toAdd + ";");
        }
        ++it;

    }

}
