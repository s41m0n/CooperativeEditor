//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_SHAREDEDITOR_H
#define COOPERATIVEEDITOR_SHAREDEDITOR_H

#include <set>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "../network/Connection.h"
#include "../components/Message.h"
#include "../crdtAlgorithm/CrdtAlgorithm.h"

/**
 * SharedEditor class, the client of this architecture
 *
 * @autor Simone Magnani - s41m0n
 */
class SharedEditor {

private:
    ///Connection to the server
    Connection conn;
    ///The specific editor ID
    unsigned int editorId;
    ///The vector of symbol, which composes the text
    std::vector<Symbol> symbols;
    ///The actual file used
    std::string currentFile;
    boost::shared_ptr<BasicMessage> bufferOut;
    boost::shared_ptr<BasicMessage> bufferIn;
    ///The unique digit generator
    unsigned int digitGenerator;
    ///Since there is no guys, used to wait until all "automated" operations are performed
    bool ready = false;
    ///Method to generate a symbol
    Symbol generateSymbol(int index, char value);

public:
    ///Classic constructor with all parameters
    SharedEditor(boost::asio::io_service& io_service,
                               const std::string& host, const std::string& service);
    ///Destructor used for debug purpose only by now
    ~SharedEditor();

    ///Method to be called once connected to the server
    void handle_connect(const boost::system::error_code& e,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    ///Method to be called once something has been written on socket
    void handle_write(const boost::system::error_code& e);
    ///Method to be called once something has been read on socket
    void handle_read(const boost::system::error_code& e);

    ///Crdt local method to insert a symbol
    void localInsert(int index, char value);
    ///Crdt local method to erase a symbol
    void localErase(int index);
    ///Method to perform the tostring on file
    void writeOnFile();
    ///Method to return the sequence of characters
    std::string to_string();

};


#endif //COOPERATIVEEDITOR_SHAREDEDITOR_H
