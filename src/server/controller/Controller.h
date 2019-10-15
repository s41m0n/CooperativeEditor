#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H


#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <queue>
#include <memory>

#include "components/messages/CrdtMessage.h"
#include "common/TcpSocket.h"
#include "server/model/Model.h"

/**
 * Controller server class
 *
 */
class Controller : public QTcpServer {

Q_OBJECT

private:
    Model *model;

    ///The queue containing all the messages
    std::queue<std::shared_ptr<CrdtMessage>> messages;

    ///The mutex used for the message queue
    std::mutex queueMutex;

    ///Method to dispatch messages
    void dispatch();

public:
    Controller(Model *model, unsigned short port, QWidget *parent = nullptr);


protected:
    void incomingConnection(qintptr handle) override;

public slots:

    ///Method called on new connection available in the Tcp server
    void onNewConnection();

    ///Method called when the state of the socket changes (disconnected, etc.)
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

    ///Method called when there is data available to read
    void onReadyRead();

};

#endif //COOPERATIVEEDITOR_CONTROLLER_H
