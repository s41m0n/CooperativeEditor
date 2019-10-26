#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <spdlog/spdlog.h>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <src/components/messages/Header.h>
#include <components/messages/BasicMessage.h>
#include <src/components/messages/FileListingMessage.h>
#include <src/components/messages/RequestMessage.h>
#include <src/components/messages/UserMessage.h>
#include <src/components/messages/FileMessage.h>
#include <src/components/messages/CrdtMessage.h>

/**
 * TcpSocket class, an extension to the QTcpSocket which include more info
 */
class TcpSocket : public QTcpSocket {

Q_OBJECT

private:

    ///The dedicated socket data stream
    QDataStream ds;

    unsigned id;

    Header header;

public:

    explicit TcpSocket(QObject *parent = nullptr);

    void setIdentifier(unsigned id);

    unsigned getIdentifier();

    bool isMessageAvailable();

    bool operator==(const TcpSocket &b) {
      return this->socketDescriptor() == b.socketDescriptor();
    }

    bool operator==(const TcpSocket *b) {
      return this->socketDescriptor() == b->socketDescriptor();
    }

    ///Operator overload to make simpler sending msg
    void sendMsg(BasicMessage &val);

    ///Operator overload to make simpler receiving msg
    BasicMessage *readMsg();

};


#endif //COOPERATIVEEDITOR_TCPSOCKET_H
