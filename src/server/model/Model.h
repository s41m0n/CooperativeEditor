#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QFile>
#include <QString>
#include <QVector>
#include <algorithm>
#include <atomic>
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>

#include "components/Symbol.h"
#include "src/include/lib/crdt/CrdtAlgorithm.h"

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

  /// The map of TcpSocket to its user structure
  std::map<TcpSocket *, User> socketToUser;

  /// Editor Id unique generator
  std::atomic<quint32> idGenerator;

public:
  Model();

  /**
   * Function to handle a CRDT Remote Insert
   * @param socket  the socket which performed the insertion
   * @param symbol  the symbol to be inserted
   */
  void userInsert(TcpSocket *socket, Symbol &symbol);

  /**
   * Function to handle a CRDT Remote Erase
   * @param socket the socket which performed the deletion
   * @param symbol the symbol to be deleted
   */
  void userErase(TcpSocket *socket, Symbol &symbol);

  /**
   * Function to remove a user connection
   * @param socket the socket to be removed
   */
  void removeConnection(TcpSocket *socket);

  /**
   * Function to create a user file
   * @param socket the socket who requires the creation
   * @param filename the file of interest
   * @return true if the file has been correctly created, false otherwise
   */
  bool createFileByUser(TcpSocket *socket, const QString &filename);

  /**
   * Function to open a user file
   * @param socket the socket who requires the opening
   * @param filename the file of interest
   * @return true if the file has been correctly opened, false otherwise
   */
  bool openFileByUser(TcpSocket *socket, const QString &filename);

  /**
   * Function to get all available files for a user
   * @param sender the socket of interest
   * @return the list of its files if any
   */
  QVector<QString> getAvailableUserFiles(TcpSocket *sender);

  /**
   * Function to retrieve the file a target socket is currently using
   * @param socket the socket of interest
   * @return the pointer to the file
   */
  std::shared_ptr<ServerFile> getFileBySocket(TcpSocket *socket);

  /**
   * Function to insert user structure into the model
   * @param socket the socket of the new client
   * @param user the structure containing all its retrieved params
   */
  void insertUserActivity(TcpSocket *socket, const User &user);

  /**
   * Function to remove user structure from the model
   * @param socket the socket which has disconnected
   */
  void removeUserActivity(TcpSocket *socket);

  /**
   * Function to retrieve the user structure from the socket
   * @param socket the socket of interest
   * @return the user structure if any
   */
  const User &getUserActivity(TcpSocket *socket);

  /**
   * Function to handle user login
   * @param user the user to be logged
   * @return true if ok, false otherwise
   */
  static bool logInUser(User &user);

  /**
   *Function to register a user
   * @param user the user structure to be registered
   * @return true if ok, false otherwise
   */
  static bool registerUser(User &user);

  /**
   * Function to update a user
   * @param user the structure with the new fields
   * @param oldPassword the old user password
   * @return true if ok, false otherwise
   */
  static bool updateUser(User &user, QString oldPassword);

  /**
   * Function to generate the invite code
   * @param sender the user who requires to generate the code
   * @param filename the file of interest
   * @return the generated code
   */
  QString generateInvite(TcpSocket *sender, const QString &filename);

  /**
   * Function to check if the invite code is correct
   * @param sender the message sender
   * @param code  the invite code
   * @param content the file content which will be filled in case it the code is
   * correct
   * @return true if correct, false otherwise
   */
  bool insertInviteCode(TcpSocket *sender, const QString &code, File &content);

  /**
   * Function to retrieve all connected users to the specific file
   * @param fileID the id of the file in interes
   * @return a vector containing all the users' TcpSocket
   */
  std::vector<TcpSocket *> getFileConnections(qint32 fileID);

  /**
   * Function to check if a user is logged or not
   * @param sender the user socket to be checked
   */
   bool isLogged(TcpSocket *sender);
};

#endif // COOPERATIVEEDITOR_MODEL_H
