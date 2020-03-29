#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <QDataStream>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <string>
#include <QCryptographicHash>
#include <QHostAddress>
#include <QImage>
#include <memory>
#include <spdlog/spdlog.h>
#include <src/components/messages/RequestMessage.h>
#include <utility>

#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/UserMessage.h"
#include "client/model/Model.h"
#include "common/TcpSocket.h"
#include "components/messages/BasicMessage.h"

/**
 * Controller class for the client
 *
 */
class Controller : public QObject {

  Q_OBJECT
  Q_DISABLE_COPY(Controller)

private:
  Model *model;
  TcpSocket *socket;

  void prepareToSend(Type type, BasicMessage &msg);

public:
  Controller(Model *model, const std::string &host, int port);

public slots:

  /// Slot to wake up when data ready on the socket
  void onMessageReceived(Header &header, QByteArray &buf);

  /// Slot to wake up when LoginRequest arrived from View
  void onLoginRequest(const QString &username, const QString &password);

  // TODO: Refactor using class USER
  /// Slot to wake up when SignUpRequest arrived from View
  void onSignUpRequest(QImage image, QString name, QString surname,
                       QString username, QString email,
                       const QString &password);

  /// Slot to wake up when user insert a char locally
  void onCharInserted(int index, const QString &value,
                      const QVector<bool> &attributes);

  /// Slot to wake up when user erase a char locally
  void onCharErased(int index, int size);

  void onCharUpdated(int index, int size, Attribute attribute, bool set);

  void onFileRequest(const QString &filename, bool exists);

  void onShowEditProfile();

    void onFileClosed();

signals:

  void connected();
  void disconnected();
  void error();

  /// Signal to notify the view about the login
  void loginResponse(bool response);

  /// Signal to notify the view that the server has sent the list of files
  void fileListing(QVector<QString> files);

  /// Signal to notify the view about the possibility to open/create a file
  void fileResult(bool result);

  /// Method to notify the view that a file has to be opened for the first time
  void loadFileText(FileText text, QString fileName, QString username,
                    unsigned int editorId);

  /// Method to notify the view a character has been inserted by a remote user
  void remoteUserInsert(int index, FileText symbols);

  /// Method to notify the view a character has been deleted by a remote user
  void remoteUserDelete(int index, int size);

  /// Method to notify the view a character has been updated by a remote user
  void remoteUserUpdate(int index, FileText  symbols);

  /// Method to send to the view the information about the user
  void userProfileInfo(QImage image, QString name, QString surname,
                       QString email, QString username);

  void remoteUserConnected(qint32 clientId, QString name);

  void remoteUserDisconnected(quint32 clientId);
};

#endif // COOPERATIVEEDITOR_CONTROLLER_H
