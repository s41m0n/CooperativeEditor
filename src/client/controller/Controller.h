//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <string>
#include <boost/asio.hpp>
#include "network/Connection.h"
#include "components/Message.h"
#include "../view/View.h"
#include "../model/Model.h"

class View;
/**
 * SharedEditor class, the client of this architecture
 *
 * @autor Simone Magnani - s41m0n
 */
class Controller {

private:

    ///The instance of the model
    Model* model;

    ///The instance of the view
    View* view;

    ///The IO service
    boost::asio::io_service io_service;

    ///Connection to the server
    Connection conn;

    ///Since there is no guys, used to wait until all "automated" operations are performed
    std::atomic_bool ready = false;

public:

    ///Classic constructor with all parameters
    Controller(const std::string& host, const std::string& service);

    ///Destructor used for debug purpose only by now
    ~Controller();

    ///Method to be called once connected to the server
    void handle_connect(const boost::system::error_code& e,
                                      boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
    ///Method to be called once something has been written on socket
    void handle_write(const boost::system::error_code& e, BasicMessage* msg);

    ///Method to be called once something has been read on socket
    void handle_read(const boost::system::error_code& e, BasicMessage* msg);

    void handle_insert(int index, char value);

    void handle_erase(int index);

    ///Method to start the controller (Which will start the view)
    void setView(View* newView);

    int start();

};


#endif //COOPERATIVEEDITOR_CONTROLLER_H
