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
#include "src/components/messages/CursorMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/RequestMessage.h"
#include "src/components/messages/UserMessage.h"
#include <src/components/messages/UserUpdateMessage.h>

/**
 * Controller server class
 *
 */
class Controller : public QTcpServer {

  Q_OBJECT

private:
  Model *model;

  /**
   * Private function to dipatch a message to all the other users connected to
   * that file
   * @param sender the user who created the message
   * @param headerType the type of the message which will be sent
   * @param header the header of the message received
   * @param message  the message to be dispatched
   */
  void dispatch(TcpSocket *sender, Type headerType, Header header,
                BasicMessage &message);

  /**
   * Function to handle the message conversion to QByteArray before sending
   * @param sender the user sender
   * @param type  the message type
   * @param msg  the message to be sent
   */
  static void prepareToSend(TcpSocket *sender, Type type, BasicMessage &msg);

public:
  Controller(Model *model, unsigned short port, QWidget *parent = nullptr);

protected:
  /**
   * Function called every time there is an incoming connection
   * @param handle the future socket identifier
   */
  void incomingConnection(qintptr handle) override;

private slots:

  /**
   * Function called when a socket is changing its state (error,...)
   * @param socketState the new state
   */
  void onSocketStateChanged(QAbstractSocket::SocketState socketState);

  /**
   * Function called to compute the received message
   * @param header  the message header
   * @param buf  the message raw buffer still to be converted
   */
  void onMessageReceived(Header &header, QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_CONTROLLER_H
