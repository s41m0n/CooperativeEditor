#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <QTcpSocket>
#include <QWidget>
#include <QDataStream>
#include <memory>
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

    ///Operator overload to make simpler sending msg
    template<typename T, typename std::enable_if<std::is_base_of<BasicMessage, T>::value>::type* = nullptr>
    void sendMsg(T &val) {
      ds << val;
    }

    ///Operator overload to make simpler receiving msg
    BasicMessage *readMsg() {
      BasicMessage base;
      ds >> base;

      switch (base.getMsgType()) {
        case Type::CONNECT : {
          auto ret = new BasicMessage(std::move(base));
          return ret;
        }
        case Type::LOGIN : {
          auto ret = new LoginMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::LISTING: {
          auto ret = new FileListingMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::CREATE: {
          auto ret = new RequestMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::OPEN: {
          auto ret = new RequestMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::FILE_RESULT: {
          auto ret = new ResultMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::LOGIN_RESULT: {
          auto ret = new ResultMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::CONTENT: {
          auto ret = new FileContentMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::INSERT: {
          auto ret = new CrdtMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::ERASE: {
          auto ret = new CrdtMessage(std::move(base));
          ds >> *ret;
          return ret;
        }
        case Type::UNKNOWN: {
          return new BasicMessage();
        }
        default:
          return new BasicMessage();
      }
    }

};


#endif //COOPERATIVEEDITOR_TCPSOCKET_H
