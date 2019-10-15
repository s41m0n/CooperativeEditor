#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <spdlog/spdlog.h>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <components/messages/BasicMessage.h>
#include <components/messages/LoginMessage.h>
#include <src/components/messages/FileListingMessage.h>
#include <src/components/messages/RequestMessage.h>
#include <src/components/messages/ResultMessage.h>
#include <src/components/messages/FileContentMessage.h>
#include <src/components/messages/CrdtMessage.h>

/**
 * TcpSocket class, an extension to the QTcpSocket which include more info
 */
class TcpSocket : public QTcpSocket {

Q_OBJECT

private:

    ///The dedicated socket data stream
    QDataStream ds;

    ///The variable to undestand if user is logged
    bool userAuthN;

public:

    ///Constructor
    explicit TcpSocket(QObject *parent = nullptr);

    ///Method to find out if user is authN
    bool isUserAuthN();

    ///Method to set user authN (or logged out)
    void setUserAuthN(bool value);

    bool operator==(const TcpSocket &b) { return this->socketDescriptor() == b.socketDescriptor();}

    bool operator==(const TcpSocket *b) { return this->socketDescriptor() == b->socketDescriptor();}

    ///Operator overload to make simpler sending msg
    template<typename T, typename std::enable_if<std::is_base_of<BasicMessage, T>::value>::type * = nullptr>
    void sendMsg(T &val) {
      //It sends the Type before the entire message, like in networking protocols.
      ds << val.getMsgType() << val;
      spdlog::debug("Sent Message!\n" + val.toStdString());
    }

    ///Operator overload to make simpler receiving msg
    BasicMessage *readMsg() {
      BasicMessage *msg = nullptr;
      Type type;
      ds >> type;

      //Depending on the Type, it will read a different message
      switch (type) {
        case Type::CONNECT : {
          msg = new BasicMessage();
          break;
        }
        case Type::LOGIN : {
          msg = new LoginMessage();
          break;
        }
        case Type::LISTING: {
          msg = new FileListingMessage();
          break;
        }
        case Type::CREATE: {
          msg = new RequestMessage();
          break;
        }
        case Type::OPEN: {
          msg = new RequestMessage();
          break;
        }
        case Type::FILE_RESULT: {
          msg = new ResultMessage();
          break;
        }
        case Type::LOGIN_RESULT: {
          msg = new ResultMessage();
          break;
        }
        case Type::CONTENT: {
          msg = new FileContentMessage();
          break;
        }
        case Type::INSERT: {
          msg = new CrdtMessage();
          break;
        }
        case Type::ERASE: {
          msg = new CrdtMessage();
          break;
        }
        case Type::UNKNOWN: {
          return msg;
        }
        default: {
          return msg;
        }
      }
      ds >> *msg;
      spdlog::debug("Received Message!\n" + msg->toStdString());
      return msg;
    }

};


#endif //COOPERATIVEEDITOR_TCPSOCKET_H
