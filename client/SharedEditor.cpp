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
        bufferIn = boost::make_shared<BasicMessage>();
        conn.async_read(bufferIn,
                               boost::bind(&SharedEditor::handle_read, this,
                                           boost::asio::placeholders::error));


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
void SharedEditor::handle_read(const boost::system::error_code& e) {

    if (!e) {
        spdlog::debug("SharedEditor{}::Received Message (type={})", editorId, bufferIn->getMsgType());

        switch (bufferIn->getMsgType()) {

            case Type::CONNECT: {
                this->editorId = bufferIn->getEditorId();
                bufferIn = boost::make_shared<FilesListingMessage>();
                conn.async_read(bufferIn,
                                boost::bind(&SharedEditor::handle_read, this,
                                            boost::asio::placeholders::error));
                return;
            }
            case Type::LISTING : {
                std::string availableFiles =  boost::static_pointer_cast<FilesListingMessage, BasicMessage>(bufferIn)->getFiles();

                if(availableFiles.empty()) {

                    currentFile = std::string("prova.txt");
                    bufferOut = boost::make_shared<RequestMessage>(Type::CREATE, editorId, currentFile);
                    conn.async_write(bufferOut,
                                     boost::bind(&SharedEditor::handle_write, this,
                                                 boost::asio::placeholders::error));
                } else {

                    currentFile = availableFiles.substr(0, availableFiles.find_last_of(';'));

                    bufferOut = boost::make_shared<RequestMessage>(Type::OPEN, editorId, currentFile);
                    conn.async_write(bufferOut,
                                     boost::bind(&SharedEditor::handle_write, this,
                                                 boost::asio::placeholders::error));
                }
                return;
            }
            case Type::FILEOK: {
                bufferIn = boost::make_shared<FileContentMessage>();
                conn.async_read(bufferIn,
                                boost::bind(&SharedEditor::handle_read, this,
                                            boost::asio::placeholders::error));
                return;
            }
            case Type::CONTENT: {
                this->symbols = boost::static_pointer_cast<FileContentMessage>(bufferIn)->getSymbols();
                this->writeOnFile();
                this->ready = true;
                break;
            }
            case Type::FILEKO : {
                throw std::runtime_error("SharedEditor::Unable to create file");
            }
            case Type::INSERT : {
                CrdtAlgorithm::remoteInsert(boost::static_pointer_cast<CrdtMessage>(bufferIn)->getSymbol(), symbols);
                break;
            }
            case Type::ERASE : {
                CrdtAlgorithm::remoteErase(boost::static_pointer_cast<CrdtMessage>(bufferIn)->getSymbol(), symbols);
                break;
            }
            default:
                throw std::runtime_error("SharedEditor::Bad message received from server");
        }

        writeOnFile();

        bufferIn = boost::make_shared<CrdtMessage>();
        conn.async_read(bufferIn,
                        boost::bind(&SharedEditor::handle_read, this,
                                    boost::asio::placeholders::error));
    }
    else
        //Error while reading (may be also socket shutdown)
        spdlog::error("SharedEditor::Read error -> {}", e.message());
}

void SharedEditor::handle_write(const boost::system::error_code& e)
{
    if(!e) {

        spdlog::debug("SharedEditor{}::Sent Message (type={})", editorId, bufferOut->getMsgType());

        if(bufferOut->getMsgType() == Type::CREATE || bufferOut->getMsgType() == Type::OPEN) {
            bufferIn = boost::make_shared<RequestMessage>();
            conn.async_read(bufferIn,
                    boost::bind(&SharedEditor::handle_read, this,
                            boost::asio::placeholders::error));
        }

    } else
        // Error while writing (same).
        spdlog::error("SharedEditor::Write error -> {}", e.message());
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

    Symbol s = generateSymbol(index, value);
    symbols.insert(symbols.begin() + index, s);

    writeOnFile();

    bufferOut = boost::make_shared<CrdtMessage>(Type::INSERT, s, editorId);
    conn.async_write(bufferOut,
            boost::bind(&SharedEditor::handle_write, this,
                    boost::asio::placeholders::error));
}

void SharedEditor::localErase(int index) {
    auto s = (index < symbols.size() && index >= 0) ? &symbols[index] : nullptr;

    if (s != nullptr) {

        while(!ready);

        symbols.erase(symbols.begin() + index);

        writeOnFile();

        bufferOut = boost::make_shared<CrdtMessage>(Type::ERASE, *s, editorId);
        conn.async_write(bufferOut,
                boost::bind(&SharedEditor::handle_write, this,
                        boost::asio::placeholders::error));
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
