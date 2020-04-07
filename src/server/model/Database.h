#ifndef COOPERATIVEEDITOR_DATABASE_H
#define COOPERATIVEEDITOR_DATABASE_H

#include "ServerFile.h"
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <src/common/File.h>
#include <src/common/User.h>
#include <string>

class Database {

private:
  const std::string databaseName = "cooperative_database.db";

  sqlite3 *DBConnection{};

  // The constructor (private because is a singleton class)
  Database();

  void createDB();

  bool checkTable();

  bool openConnection();

  //Method to convert a QImage into a QByteArray
  static QByteArray convertFromQImage(const QImage &userPic);

  //Method to recreate a QImage given a QByteArray
  static QImage convertIntoQImage(QByteArray buffer);

  static QByteArray convertFromFileText(FileText &text);

  static FileText convertIntoFileText(QByteArray buffer);

  // Callback method used to check if the User table exists
  static int callbackCount(void *data, int colNum, char **dataRow,
                           char **colName);

  static void printErrorAndExit(const std::string& error = "Unable to connect to DB");

public:
  Database(const Database &) = delete;

  void operator=(const Database &) = delete;

  // Static method to get the singleton instance
  static Database &getInstance() {
    static Database DBinstance;
    return DBinstance;
  }

  bool insertUser(User &user);

  bool loginUser(User &user);

  bool updateUser(User &user);

  bool deleteUser(User user);

  bool checkInvite(const QString& link, QString &userInviting, QString &filename, QString &userInvited);

  bool insertInvite(const QString& link);

  QVector<QString> getUserFiles(User &user);

  int getUserFileID(const QString& filename, User &user);

  bool insertFile(User &user, const QString &file, std::shared_ptr<ServerFile> &serverFile);

  bool openFile(int fileID, const QString &name, std::shared_ptr<ServerFile> &serverFile);

  bool updateFile(ServerFile &file);
};

#endif // COOPERATIVEEDITOR_DATABASE_H
