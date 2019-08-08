//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H


#include <queue>
#include <map>
#include <string>
#include <boost/asio.hpp>
#include "components/Message.h"
#include "network/Connection.h"
#include "../model/Model.h"

/**
 * NetworkServer class, the server in this architecture
 *
 * @author Simone Magnani - s41m0n
 */
class Controller {

private:
    ///The model instance
    Model* model;

    ///The IO SERVICE
    boost::asio::io_service io_service;

    ///Map of connection ID, filename required by user and Connection ptr
    std::map<int, connection_ptr> connections;

    ///The queue containing all the messages
    std::queue<CrdtMessage*> messages;

    ///The mutex used for the message queue
    std::mutex queueMutex;

    ///The mutex used for the connection map
    std::mutex connectionsMutex;

    /// The acceptor object used to accept incoming socket connections.
    boost::asio::ip::tcp::acceptor acceptor_;

    ///Method to dispatch messages
    void dispatch();

    /// Handle completion of a accept operation.
    void handle_accept(const boost::system::error_code &e, connection_ptr &conn);

    /// Handle completion of a read operation.
    void handle_read(const boost::system::error_code &e, int connId, BasicMessage* msg);

    /// Handle completion of a write operation.
    void handle_write(const boost::system::error_code &e, int connId, BasicMessage* msg);

public:
    ///Class constructor, given an io_service and a port
    Controller(Model* model, unsigned short port);

    ///Class destructor (actually used to debug)
    ~Controller();

    ///Method to start the IO
    int start();
};

#endif //COOPERATIVEEDITOR_CONTROLLER_H
