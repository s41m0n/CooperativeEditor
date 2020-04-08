#ifndef COOPERATIVEEDITOR_DATABASE_H
#define COOPERATIVEEDITOR_DATABASE_H

#include "ServerFile.h"
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <src/common/File.h>
#include <src/common/User.h>
#include <string>

/**
 * Database singleton class to handle all interactions with SQLITE3
 */
class Database {

private:
  const std::string databaseName = "cooperative_database.db";

  sqlite3 *DBConnection{};

  Database();

  /**
   * Function to create the database if it doesn't exist yet
   */
  void createDB();

  /**
   * Function to check if all the needed tables are present
   * @return true if yes, false otherwise
   */
  bool checkTable();

  /**
   * Function to open a connection to the DB
   * @return true if correctly opened, false otherwise
   */
  bool openConnection();

  /**
   * Function to convert a QImage into a QByteArray
   * @param userPic the user image to be converted
   * @return the raw byte array
   */
  static QByteArray convertFromQImage(const QImage &userPic);

  /**
   * Function to recreate a QImage given a QByteArray
   * @param buffer the raw buffer
   * @return the converted Image
   */
  static QImage convertIntoQImage(QByteArray buffer);

  /**
   * Convert a FileText into raw bytes
   * @param text the filetext to be converted
   * @return the raw bytes
   */
  static QByteArray convertFromFileText(FileText &text);

  /**
   * Funcion to recreate a FileText from a raw buffer
   * @param buffer the raw bytes
   * @return the FileText
   */
  static FileText convertIntoFileText(QByteArray buffer);

  /**
   * Function to be passed to sqlite3_exec as callback
   * @param data
   * @param colNum
   * @param dataRow
   * @param colName
   * @return
   */
  static int callbackCount(void *data, int colNum, char **dataRow,
                           char **colName);

  /**
   * Function to print errors and exit due to a relevant problem
   * @param error the string to be printed
   */
  static void printErrorAndExit(const std::string& error = "Unable to connect to DB");

public:
  Database(const Database &) = delete;

  void operator=(const Database &) = delete;

  /**
   * Function to return the singleton instance
   * @return the singleton DB instance
   */
  static Database &getInstance() {
    static Database DBinstance;
    return DBinstance;
  }

  /**
   * Function to insert a user into the DB
   * @param user the user structure
   * @return true if ok, false otherwise
   */
  bool insertUser(User &user);

  /**
   * Function to log in a user
   * @param user the user structure
   * @return true if ok, false otherwise
   */
  bool loginUser(User &user);

  /**
   * Function to update a user into the DB
   * @param user the user structure
   * @return true if ok, false otherwise
   */
  bool updateUser(User &user);

  /**
   * Function to check if an invite code is valid and give the invited user the right permission
   * @param link the invite code
   * @param userInviting the user who created the invite code
   * @param filename  the file of interest
   * @param userInvited the invited user who is attempting to get privileges
   * @return true if ok, false otherwise
   */
  bool checkInvite(const QString& link, QString &userInviting, QString &filename, QString &userInvited);

  bool insertInvite(const QString& link);

  QVector<QString> getUserFiles(User &user);

  int getUserFileID(const QString& filename, User &user);

  bool insertFile(User &user, const QString &file, std::shared_ptr<ServerFile> &serverFile);

  bool openFile(int fileID, const QString &name, std::shared_ptr<ServerFile> &serverFile);

  bool updateFile(ServerFile &file);
};

#endif // COOPERATIVEEDITOR_DATABASE_H
