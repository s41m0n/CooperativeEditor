#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QString>
#include <QVector>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <QFile>
#include <algorithm>
#include <filesystem>

#include "src/include/lib/crdt/CrdtAlgorithm.h"
#include "components/Symbol.h"

#include "Database.h"
#include "ServerFile.h"
#include "common/TcpSocket.h"
#include "common/User.h"

/**
 * Model server class
 */
class Model {

private:

  /// The map of the current file for each user
  std::multimap<std::shared_ptr<ServerFile>, TcpSocket *> usersFile;

  /// The mutex for the usersFile map
  std::mutex usersFileMutex;

  std::map<TcpSocket *, User> socketToUser;

  /// Editor Id unique generator
  std::atomic<unsigned> idGenerator;

public:
  /// Classic constructor
  Model();

  /// Method to handle user remote insertion
  void userInsert(TcpSocket *socket, Symbol& symbol);

  /// Method to handle user remote deletion
  void userErase(TcpSocket *socket, Symbol& symbol);

  void removeConnection(TcpSocket *socket);

  /// Method called when a user requests to create a file
  bool createFileByUser(TcpSocket *socket, const QString &filename);

  /// Method called when a user requests to open a file
  bool openFileByUser(TcpSocket *socket, const QString& filename);

  /// Returns the list (string) of all user available files
  static QVector<QString> getAvailableUserFiles(User &user);

  /// Returns the list of symbols for the file the user connId has requested
  std::shared_ptr<ServerFile> getFileBySocket(TcpSocket *socket);

  void insertUserActivity(TcpSocket *socket, const User& user);

  void removeUserActivity(TcpSocket *socket);

  const User &getUserActivity(TcpSocket *socket);

  /// Returns a bool to indicate if the logIn credential are correct and set the
  /// User object
  static bool logInUser(User &user);

  /// Returns a bool to indicate if the registration was successful
  static bool registerUser(User &user);

  /// Returns a bool to indicate if the update was successful
  static bool updateUser(User &user);

  /// Returns a bool to indicate if the update was successful
  static bool deleteUser(User &user);

  /// Return a bool to indicate if the invite has been correctly inserted
  QString generateInvite(TcpSocket *sender, const QString &filename);

  bool insertInviteCode(TcpSocket *sender, const QString &code, File &content);

  std::vector<TcpSocket *> getFileConnections(int fileID);
};

#endif // COOPERATIVEEDITOR_MODEL_H
