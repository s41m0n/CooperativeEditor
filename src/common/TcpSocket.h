#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <QDataStream>
#include <QObject>
#include <QTcpSocket>
#include <src/components/messages/Header.h>
#include <spdlog/spdlog.h>

/**
 * TcpSocket class, an extension to the QTcpSocket which include more info
 */
class TcpSocket : public QTcpSocket {

  Q_OBJECT
  Q_DISABLE_COPY(TcpSocket)

signals:
  void messageReceived(Header &header, QByteArray &msg);
private slots:
  void onReadyRead();

private:
  qint32 clientID;

  Header header;

  bool isMessageAvailable();

public:
  explicit TcpSocket(QObject *parent = nullptr);

  void sendMsg(Header &headerToSend, QByteArray &val);

  void setClientID(qintptr handle);

  qint32 getClientID();

  bool operator==(const TcpSocket &b) {
    return this->clientID == b.clientID;
  }

  bool operator==(const TcpSocket *b) {
    return this->clientID == b->clientID;
  }
};

#endif // COOPERATIVEEDITOR_TCPSOCKET_H
