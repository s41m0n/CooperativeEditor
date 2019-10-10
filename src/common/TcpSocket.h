#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <QTcpSocket>
#include <QWidget>
#include <QDataStream>

/**
 * TcpSocket class, an extension to the QTcpSocket which include more info
 */
class TcpSocket : public QTcpSocket{

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

    ///Operator overload to make simpler reading msg
    template <class T>
    void operator<<(const T &val) {
      ds << val;
    }

    ///Operator overload to make simpler sending msg
    template <class T>
    void operator>>(T &val) {
      ds >> val;
    }

};


#endif //COOPERATIVEEDITOR_TCPSOCKET_H
