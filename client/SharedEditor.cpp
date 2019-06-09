//class SharedEditor;
// Created by s41m0n on 16/05/19.
//

#include <spdlog/spdlog.h>
#include <fstream>
#include "SharedEditor.h"

SharedEditor::SharedEditor(boost::asio::io_service& io_service,
const std::string& host, const std::string& service)
: conn(io_service), editorId(0), digitGenerator(0), currentFile() {

    // Resolve the host name into an IP address.
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(host, service);
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;

    //Connect asynchronously
    conn.socket().async_connect(endpoint,
            boost::bind(&SharedEditor::handle_connect, this,
        boost::asio::placeholders::error, ++endpoint_iterator));
    spdlog::debug("SharedEditor{}::Created", editorId);
}


SharedEditor::~SharedEditor() {
    spdlog::debug("SharedEditor{0:d}::Destroyed", editorId);
}

/// Handle completion of a connect operation.
void SharedEditor::handle_connect(const boost::system::error_code& e,
                    boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
    if (!e) {

        //Successfully connect, receive editor ID
        auto msg = new BasicMessage();
        conn.async_read(*msg,
                boost::bind(&SharedEditor::handle_read, this,
                        boost::asio::placeholders::error, msg));


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
void SharedEditor::handle_read(const boost::system::error_code& e, BasicMessage* msg) {

    if (!e) {
        spdlog::debug("SharedEditor{}::Received Message (type={})", editorId, msg->getMsgType());

        switch (msg->getMsgType()) {

            case Type::CONNECT: {
                this->editorId = msg->getEditorId();
                auto newMsg = new FilesListingMessage();
                conn.async_read(*newMsg,
                        boost::bind(&SharedEditor::handle_read, this,
                                boost::asio::placeholders::error, newMsg));
                break;
            }
            case Type::LISTING : {
                std::string availableFiles = dynamic_cast<FilesListingMessage*>(msg)->getFiles();

                if(availableFiles.empty()) {

                    currentFile = std::string("prova.txt");
                    auto newMsg = new RequestMessage(Type::CREATE, editorId, currentFile);
                    conn.async_write(*newMsg,
                            boost::bind(&SharedEditor::handle_write, this,
                                    boost::asio::placeholders::error, newMsg));
                } else {

                    currentFile = availableFiles.substr(0, availableFiles.find_last_of(';'));

                    auto newMsg = new RequestMessage(Type::OPEN, editorId, currentFile);
                    conn.async_write(*newMsg,
                            boost::bind(&SharedEditor::handle_write, this,
                                    boost::asio::placeholders::error, newMsg));
                }
                break;
            }
            case Type::FILEOK: {
                auto newMsg = new FileContentMessage();
                conn.async_read(*newMsg,
                        boost::bind(&SharedEditor::handle_read, this,
                                        boost::asio::placeholders::error, newMsg));
                break;
            }
            case Type::CONTENT: {
                symbols = dynamic_cast<FileContentMessage*>(msg)->getSymbols();
                spdlog::debug("Received content -> {}", to_string());
                ready = true;
                break;
            }
            case Type::FILEKO : {
                throw std::runtime_error("SharedEditor::Unable to create file");
            }
            case Type::INSERT : {
                CrdtAlgorithm::remoteInsert(dynamic_cast<CrdtMessage*>(msg)->getSymbol(), symbols);
                writeOnFile();
                auto newMsg = new CrdtMessage();
                conn.async_read(*newMsg,
                              boost::bind(&SharedEditor::handle_read, this,
                                          boost::asio::placeholders::error, newMsg));
                break;
            }
            case Type::ERASE : {
                CrdtAlgorithm::remoteErase(dynamic_cast<CrdtMessage*>(msg)->getSymbol(), symbols);
                writeOnFile();
                auto newMsg = new CrdtMessage();
                conn.async_read(*newMsg,
                              boost::bind(&SharedEditor::handle_read, this,
                                          boost::asio::placeholders::error, newMsg));
                break;
            }
            default:
                throw std::runtime_error("SharedEditor::Bad message received from server");
        }

    }
    else
        //Error while reading (may be also socket shutdown)
        spdlog::error("SharedEditor::Read error -> {}", e.message());
    delete msg;
}

void SharedEditor::handle_write(const boost::system::error_code& e, BasicMessage* msg)
{
    if(!e) {

        spdlog::debug("SharedEditor{}::Sent Message (type={})", editorId, msg->getMsgType());

        if(msg->getMsgType() == Type::CREATE || msg->getMsgType() == Type::OPEN) {
            auto newMsg = new RequestMessage();
            conn.async_read(*newMsg,
                    boost::bind(&SharedEditor::handle_read, this,
                            boost::asio::placeholders::error, newMsg));
        }

    } else
        // Error while writing (same).
        spdlog::error("SharedEditor::Write error -> {}", e.message());
    delete msg;
}

void SharedEditor::writeOnFile() {
    std::ofstream outfile;
    std::string filename = "SE" + std::to_string(editorId) + currentFile ;
    outfile.open(filename);
    outfile << to_string() << std::endl;
}

std::string SharedEditor::to_string() {
    std::string str;
    std::for_each(symbols.begin(), symbols.end(), [&str](Symbol s){
        str += s.getChar();
    });
    return str;
}

void SharedEditor::localInsert(int index, char value) {

    while(!ready);

    symbols.insert(symbols.begin() + index, generateSymbol(index, value));

    writeOnFile();

    auto newMsg = new CrdtMessage(Type::INSERT, symbols[index], editorId);
    conn.async_write(*newMsg,
            boost::bind(&SharedEditor::handle_write, this,
                    boost::asio::placeholders::error, newMsg));
}

void SharedEditor::localErase(int index) {
    auto s = (index < symbols.size() && index >= 0) ? &symbols[index] : nullptr;

    if (s != nullptr) {

        while(!ready);

        symbols.erase(symbols.begin() + index);

        writeOnFile();

        auto newMsg = new CrdtMessage(Type::ERASE, *s, editorId);
        conn.async_write(*newMsg,
                boost::bind(&SharedEditor::handle_write, this,
                        boost::asio::placeholders::error, newMsg));
    }
}

Symbol SharedEditor::generateSymbol(int index, char value) {
    auto symbolBefore = (index-1 < symbols.size() && index - 1 >= 0 && !symbols[index - 1].getPos().empty()) ? &symbols[index - 1] : nullptr;
    auto symbolAfter = (index < symbols.size() && index >= 0 && !symbols[index].getPos().empty()) ? &symbols[index] : nullptr;
    std::vector<int> newPos;
    CrdtAlgorithm::generatePosBetween(symbolBefore, symbolAfter, &newPos);
    Symbol s(value, editorId, digitGenerator++, newPos);
    return s;
}
