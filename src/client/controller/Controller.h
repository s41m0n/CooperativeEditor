//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <string>
#include <QTcpSocket>
#include <QMainWindow>

#include "components/Message.h"
#include "client/view/View.h"
#include "client/model/Model.h"

//Forward class declaration
class View;

/**
 * Controller class for the client
 *
 * @autor Simone Magnani - s41m0n
 */
class Controller : QMainWindow {

Q_OBJECT

private:
    ///The instance of the model
    Model *model;

    ///The instance of the view
    View *view;

    ///The QTcpSOcket
    QTcpSocket _socket;

public:
    ///Classic constructor
    Controller(Model *model, const std::string &host, int port);

    ///Destructor used for debug purpose only by now
    ~Controller();

    ///Method to handle a CrdtMessage - symbol inserted
    void handle_insert(int index, char value);

    ///Method to handle a CrdtMessage - symbol erased
    void handle_erase(int index);

    ///Method to start the controller (Which will start the view)
    void setView(View *newView);

public slots:

    ///Method to be called once something has been read on socket
    void onReadyRead();

};


#endif //COOPERATIVEEDITOR_CONTROLLER_H
