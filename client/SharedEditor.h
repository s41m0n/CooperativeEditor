//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_SHAREDEDITOR_H
#define COOPERATIVEEDITOR_SHAREDEDITOR_H

#include "../network/Connection.h"
#include "../components/Message.h"
#include "../components/Symbol.h"

///Forward declaration of class NetworkServer (to avoid cycle)
class NetworkServer;

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
    int editorId;
    ///The vector of symbol, which composes the text
    std::vector<Symbol> symbols;
    ///The unique digit generator
    int _counter = 0;

    Symbol generateSymbol(int index, char value);

public:
    SharedEditor(boost::asio::io_service& io_service,
                               const std::string& host, const std::string& service);
    ~SharedEditor();
    void localInsert(int index, char value);
    void localErase(int index);
    void remoteInsert(Symbol s);
    void remoteErase(Symbol s);
    void process(std::shared_ptr<Message>& msg);
    void handle_connect(const boost::system::error_code& e,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    void handle_write(const boost::system::error_code& e, std::shared_ptr<Message>& msg);
    void handle_read(const boost::system::error_code& e, std::shared_ptr<Message>& incomingMsg);
    void writeOnFile();
    std::string to_string();
};


#endif //COOPERATIVEEDITOR_SHAREDEDITOR_H
