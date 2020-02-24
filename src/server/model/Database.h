#ifndef COOPERATIVEEDITOR_DATABASE_H
#define COOPERATIVEEDITOR_DATABASE_H

#include <sqlite3.h>
#include <src/common/User.h>
#include <string>

class Database {

private:
  const std::string databaseName = "cooperative_database.db";

  sqlite3 *DBConnection{};

  // The constructor (private because is a singleton class)
  Database();

  void createTableUser();

  bool checkTable();

  bool openConnection();

  //Method to convert a QImage into a QByteArray
  static QByteArray convertFromQImage(const QImage &userPic);

  //Method to recreate a QImage given a QByteArray
  static QImage convertIntoQImage(QByteArray buffer);

  // Callback method used to check if the User table exists
  static int callbackCount(void *data, int colNum, char **dataRow,
                           char **colName);

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
};

#endif // COOPERATIVEEDITOR_DATABASE_H
