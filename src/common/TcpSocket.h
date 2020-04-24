#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <QDataStream>
#include <QObject>
#include <QTcpSocket>
#include <spdlog/spdlog.h>
#include <src/components/messages/Header.h>

/**
 * TcpSocket class, an extension to the QTcpSocket which include more info
 */
class TcpSocket : public QTcpSocket {

  Q_OBJECT
  Q_DISABLE_COPY(TcpSocket)

signals:
  /**
   * Signal emitted only when a message has been entirely read
   * @param header the message header
   * @param msg  the message
   */
  void messageReceived(Header &header, QByteArray &msg);

private slots:
  /**
   * Slot called when data available
   */
  void onReadyRead();

private:
  QDataStream ds;

  quint32 clientID;

  Header header;

  /**
   * Function to check if the data to be read fill the message size
   * @return true if ok, false otherwise
   */
  bool isMessageAvailable();

public:
  explicit TcpSocket(QObject *parent = nullptr);

  bool operator==(const TcpSocket &b) { return this->clientID == b.clientID; }

  bool operator==(const TcpSocket *b) { return this->clientID == b->clientID; }

  /**
   * Function to send a message through that socket
   * @param headerToSend  the header of the message
   * @param val  the message as raw bytes
   */
  void sendMsg(Header &headerToSend, QByteArray &val);

  void setClientID(qintptr handle);

  quint32 getClientID();
};

#endif // COOPERATIVEEDITOR_TCPSOCKET_H
