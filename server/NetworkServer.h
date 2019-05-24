//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_NETWORKSERVER_H
#define COOPERATIVEEDITOR_NETWORKSERVER_H


#include <queue>
#include <memory>
#include <map>
#include "../components/Message.h"
#include "../client/SharedEditor.h"

/**
 * NetworkServer class, the server in this architecture
 *
 * @author Simone Magnani - s41m0n
 */
class NetworkServer {

private:
    ///The map of the connection, containing the editorId and the pointer to the connection
    std::map<int, connection_ptr> connections;
    ///The queue containing all the messages
    std::queue<std::shared_ptr<Message>> messages;
    ///The mutex used for the message queue
    std::mutex queueMutex;
    ///The mutex used to update the symbols array
    std::mutex symbolsMutes;
    ///The mutex used for the connections map
    std::mutex connectionMapMutex;
    int idGenerator = 0;
    ///The vector of symbol, which composes the text
    std::vector<Symbol> symbols;
    /// The acceptor object used to accept incoming socket connections.
    boost::asio::ip::tcp::acceptor acceptor_;

public:
    ///Class constructor, given an io_service and a port
    NetworkServer(boost::asio::io_service &io_service, unsigned short port);

    ///Class destructor (actually used to debug)
    ~NetworkServer();

    ///Method to dispatch messages
    void dispatch();

    /// Handle completion of a accept operation.
    void handle_accept(const boost::system::error_code &e, connection_ptr &conn);

    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code &e, int connId, std::shared_ptr<Message> &msg);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code &e, int connId, std::shared_ptr<Message> &msg);

    void remoteInsert(Symbol s);

    void remoteErase(Symbol s);

    void writeOnFile();

    std::string to_string();
};

#endif //COOPERATIVEEDITOR_NETWORKSERVER_H
