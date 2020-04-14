#ifndef COOPERATIVEEDITOR_CONTROLLER_H
#define COOPERATIVEEDITOR_CONTROLLER_H

#include <QDataStream>
#include <QHostAddress>
#include <QImage>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <utility>

#include "client/model/Model.h"
#include "common/TcpSocket.h"
#include "src/components/messages/BasicMessage.h"
#include "src/components/messages/CrdtMessage.h"
#include "src/components/messages/CursorMessage.h"
#include "src/components/messages/FileListingMessage.h"
#include "src/components/messages/FileMessage.h"
#include "src/components/messages/UserMessage.h"
#include <src/components/messages/RequestMessage.h>

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

  /**
   * Function to send a message through the socket
   * @param type  the message type to be sent
   * @param msg  the message to be sent
   */
  void prepareToSend(Type type, BasicMessage &msg);

public:
  Controller(Model *model, const std::string &host, int port);

public slots:

  /**
   * Slot called when a message is available
   * @param header  the message header
   * @param buf the message as raw bytes
   */
  void onMessageReceived(Header &header, QByteArray &buf);

  /**
   * Slot to handle a login request
   * @param username  the username inserted
   * @param password  the password inserted (will be hashed)
   */
  void onLoginRequest(const QString &username, const QString &password);

  /**
   * Slot to handle a signup request
   * @param user  the structure containing all the user parameters
   */
  void onSignUpRequest(User user);

  /**
   * Slot to handle a local symbol insertion
   * @param index the index of the symbol inserted
   * @param value the character
   * @param format  the current character format
   */
  void onCharInserted(int index, QChar value, QTextCharFormat format);

  /**
   * Slot to handle a local symbol deletion
   * @param index  the index of the symbol
   */
  void onCharErased(int index);

  /**
   * Slot to handle a file request
   * @param filename  the requested filename
   * @param exists true if open, false if create
   */
  void onFileRequest(const QString &filename, bool exists);

  /**
   * Slot to handle the show profile view opening
   */
  void onShowEditProfile();

  /**
   * Slot to handle the file view closure
   */
  void onFileClosed();

  /**
   * Slot to handle the invite code insertion
   * @param code  the invite code
   */
  void onInsertInviteCode(QString code);

  /**
   * Slot to handle a generate link request
   */
  void onGenerateLink();

  /**
   * Slot to handle a local cursor position changed
   * @param position the new position
   */
  void onCursorChanged(int position);

  /**
   * Slot to handle file list request
   */
  void onRequestFileList();

  /**
   * Slot to handle the request of the text inserted by an user
   */
  void onUserTextAsked(quint32 clientId);

  /**
  * Slot to handle the request of the original text inserted by an user
  */
  void onUserOriginalTextAsked(quint32 clientId);

signals:

  /**
   * Connected signal
   */
  void connected();

  /**
   * Disconnected signal
   */
  void disconnected();

  /**
   * Error signal
   */
  void error();

  /**
   * Signal to notify the view of a login answer
   * @param response true if ok, false otherwise
   */
  void loginResponse(bool response);

  /**
   * Signal to notify the view of the list of the current user files
   * @param files the file list
   */
  void fileListing(QVector<QString> &files);

  /**
   * Signal to notify the view about a file opening/creation operation
   * @param result  true if ok, false otherwise
   */
  void fileResult(bool result);

  /**
   * Signal to notify the view of the current file content
   * @param text the file content
   * @param fileName the file name
   * @param username the username of the current user
   * @param editorId the current user editor ID
   */
  void loadFileText(FileText &text, QString &fileName, QString &username,
                    quint32 editorId);

  /**
   * Signal to notify the view of a remote user insertion
   * @param index  the index of the symbol to be inserted
   * @param symbols  the symbol to be inserted
   */
  void remoteUserInsert(int index, Symbol &symbols);

  /**
   * Signal to notify the view of a remote deletion
   * @param index  the index of the remote deletion
   */
  void remoteUserDelete(int index);

  /**
   * Signal to notify the view about all the current user info
   * @param image
   * @param name
   * @param surname
   * @param email
   * @param username
   */
  void userProfileInfo(User &user);

  /**
   * Signal to notify the view of a remote user connected
   * @param clientId  the remote clientId
   * @param name  the remote user name
   */
  void remoteUserConnected(qint32 clientId, QString &name);

  /**
   * Signal to notify the view of a remote user disconnect
   * @param clientId the remote clientId
   */
  void remoteUserDisconnected(quint32 clientId);

  /**
   * Signal to notify the view about a invite link generation answer from server
   * @param code  the generated invite link
   */
  void generateLinkAnswer(QString code);

  /**
   * Function to generate the view about a remote user cursor changed
   * @param editorId  the remote user editorID
   * @param position  the new position
   */
  void userCursorChanged(quint32 editorId, int position);

  /**
   * Signal to send to the view the text inserted by the user
   * @param positions  positions of char inserted by the user
   * @param clientId   id of the user
   */
  void sendUserText(QList<int> positions, quint32 clientId);

  /**
    * Signal to send to the view the original text inserted by the user
    * @param textAndColors  map with positions and background colors
    */
  void sendUserOriginalText(QMap<int, QBrush> textAndColors);
};

#endif // COOPERATIVEEDITOR_CONTROLLER_H
