#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H


#include <queue>
#include <map>
#include <string>
#include <QTcpServer>
#include <QTcpSocket>
#include <QObject>

#include "components/Message.h"
#include "server/model/Model.h"

/**
 * Controller server class
 *
 */
class Controller : public QObject {

Q_OBJECT

private:
    ///The model instance
    Model *model;

    ///The server
    QTcpServer _server;

    ///Map of connection ID, filename required by user and QTcpSockets
    std::map<QTcpSocket *, unsigned int> connections;

    ///The queue containing all the messages
    std::queue<CrdtMessage> messages;

    ///The mutex used for the message queue
    std::mutex queueMutex;

    ///The mutex used for the connection map
    std::mutex connectionsMutex;

    ///Method to dispatch messages
    void dispatch();

public:
    ///Class constructor, given an io_service and a port
    Controller(Model *model, unsigned short port);

    ///Class destructor (actually used to debug)
    ~Controller() override;

public slots:

    ///Method called on new connection available in the Tcp server
    void onNewConnection();

    ///Method called when the state of the socket changes (disconnected, etc.)
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

    ///Method called when there is data available to read
    void onReadyRead();

};

#endif //COOPERATIVEEDITOR_CONTROLLER_H
