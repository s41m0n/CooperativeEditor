#ifndef COOPERATIVEEDITOR_TCPSOCKET_H
#define COOPERATIVEEDITOR_TCPSOCKET_H

#include <QDataStream>
#include <QObject>
#include <QTcpSocket>
#include <components/messages/BasicMessage.h>
#include <spdlog/spdlog.h>
#include <src/components/messages/CrdtMessage.h>
#include <src/components/messages/FileListingMessage.h>
#include <src/components/messages/FileMessage.h>
#include <src/components/messages/Header.h>
#include <src/components/messages/RequestMessage.h>
#include <src/components/messages/UserMessage.h>

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
  /// The dedicated socket data stream
  QDataStream ds;

  unsigned id;

  Header header;

  bool isMessageAvailable();

public:
  explicit TcpSocket(QObject *parent = nullptr);

  void setIdentifier(unsigned id);

  unsigned getIdentifier();

  void sendMsg(Header &headerToSend, QByteArray &val);

  bool operator==(const TcpSocket &b) { return this->id == b.id; }

  bool operator==(const TcpSocket *b) { return this->id == b->id; }
};

#endif // COOPERATIVEEDITOR_TCPSOCKET_H
