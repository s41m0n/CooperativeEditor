//class SharedEditor;
// Created by s41m0n on 16/05/19.
//

#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>
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

    if (!e)
        this->process(incomingMsg);
    else
        //Error while reading (may be also socket shutdown)
        spdlog::error("SharedEditor::Read error -> {}", e.message());
}

void SharedEditor::handle_write(const boost::system::error_code& e, std::shared_ptr<Message>& msg)
{
    if(!e)
        spdlog::debug("SharedEditor{}::Sent Message (type={})", editorId, msg->getMsgType());
    else
        // Error while writing (same).
        spdlog::error("SharedEditor::Write error -> {}", e.message());
}

void SharedEditor::process(std::shared_ptr<Message>& msg) {

    if(msg) {
        switch (msg->getMsgType()) {
            case CONNECT:
                this->editorId = msg->getEditorId();
                conn.async_read(this->symbols,
                                boost::bind(&SharedEditor::handle_read, this,
                                            boost::asio::placeholders::error, std::shared_ptr<Message>()));
                spdlog::debug("SharedEditor{}::Received Message (type={}, editorId={})", editorId, msg->getMsgType(), msg->getEditorId());
                return;
            case INSERT :
                this->remoteInsert(msg->getSymbol());
                break;
            case ERASE :
                this->remoteErase(msg->getSymbol());
                break;
            default:
                throw std::runtime_error("SharedEditor::Bad message received from server");
        }
        spdlog::debug("SharedEditor{}::Received Message (type={}, editorId={})", editorId, msg->getMsgType(), msg->getEditorId());
    } else {
        spdlog::debug("SharedEditor{}::Received vector of Symbol", editorId);
        this->ready = true;
    }

    std::shared_ptr<Message> newMsg(new Message());
    conn.async_read(*newMsg,
                    boost::bind(&SharedEditor::handle_read, this,
                                boost::asio::placeholders::error, newMsg));
}

void SharedEditor::writeOnFile() {
    std::ofstream outfile;
    std::string filename = "../../testSE" + std::to_string(editorId) + ".txt";
    outfile.open(filename, std::ios_base::app);
    outfile << this->to_string() << std::endl;
}

std::string SharedEditor::to_string() {
    std::string str;
    std::for_each(this->symbols.begin(), this->symbols.end(), [&str](Symbol s){
        str += s.getChar();
    });
    return str;
}

void SharedEditor::localInsert(int index, char value) {
    while(!ready);
    Symbol s = this->generateSymbol(index, value);
    this->symbols.insert(this->symbols.begin() + index, s);
    std::shared_ptr<Message> msg(new Message(INSERT, s, this->editorId));
    this->writeOnFile();
    conn.async_write(*msg,
                     boost::bind(&SharedEditor::handle_write, this,
                                 boost::asio::placeholders::error, msg));
}

void SharedEditor::localErase(int index) {
    auto s = (index < this->symbols.size() && index >= 0) ? &this->symbols[index] : nullptr;
    if (s != nullptr) {
        while(!ready);
        this->symbols.erase(this->symbols.begin() + index);
        std::shared_ptr<Message> msg(new Message(ERASE, *s, this->editorId));
        this->writeOnFile();
        conn.async_write(*msg,
                          boost::bind(&SharedEditor::handle_write, this,
                                      boost::asio::placeholders::error, msg));
    }
}

void SharedEditor::remoteErase(Symbol s) {
    int index = CrdtAlgorithm::findPositionErase(std::move(s), this->symbols);
    if(index >= 0) {
        this->symbols.erase(this->symbols.begin() + index);
        this->writeOnFile();
    }
}

void SharedEditor::remoteInsert(Symbol s) {
    int index = CrdtAlgorithm::findPositionInsert(s, this->symbols);

    this->symbols.insert(this->symbols.begin()+index, std::move(s));
    this->writeOnFile();
}

Symbol SharedEditor::generateSymbol(int index, char value) {
    auto symbolBefore= (index-1 < this->symbols.size() && index - 1 >= 0 && !this->symbols[index - 1].getPos().empty()) ? &this->symbols[index - 1] : nullptr;
    auto symbolAfter= (index < this->symbols.size() && index >= 0 && !this->symbols[index].getPos().empty()) ? &this->symbols[index] : nullptr;
    std::vector<int> newPos;
    CrdtAlgorithm::generatePosBetween(symbolBefore, symbolAfter, &newPos);
    Symbol s(value, this->editorId, this->_counter++, newPos);
    return s;
}
