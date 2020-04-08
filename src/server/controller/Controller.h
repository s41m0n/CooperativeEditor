#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QWidget>
#include <memory>
#include <queue>
#include <spdlog/spdlog.h>

#include "common/TcpSocket.h"
#include "components/messages/CrdtMessage.h"
#include "server/model/Model.h"
#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/RequestMessage.h"
#include "src/components/messages/UserMessage.h"
#include "src/components/messages/CursorMessage.h"

/**
 * Controller server class
 *
 */
class Controller : public QTcpServer {

  Q_OBJECT

private:
  Model *model;

  void dispatch(TcpSocket *sender, Type headerType, Header header,
                BasicMessage &message);

  static void prepareToSend(TcpSocket *sender, Type type, BasicMessage &msg);

public:
  Controller(Model *model, unsigned short port, QWidget *parent = nullptr);

protected:
  /// Method called on new connection available in the Tcp server
  void incomingConnection(qintptr handle) override;

private slots:

  /// Method called when the state of the socket changes (disconnected, etc.)
  void onSocketStateChanged(QAbstractSocket::SocketState socketState);

  /// Method called when there is data available to read
  void onMessageReceived(Header &header, QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_CONTROLLER_H
