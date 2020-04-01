#include "Database.h"

Database::Database() {
  if (openConnection()) {
    if (!checkTable()) {
      createDB();
    }
    sqlite3_close(this->DBConnection);
  } else {
    spdlog::error("Error opening connection with database");
  }
}

void Database::createDB() {
  std::string sql = "CREATE TABLE User ("
                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "Username VARCHAR NOT NULL UNIQUE,"
                    "Password VARCHAR NOT NULL,"
                    "Name VARCHAR,"
                    "Surname VARCHAR,"
                    "Email VARCHAR NOT NULL UNIQUE,"
                    "ProfilePic BLOB);"

                    "CREATE TABLE Invites ("
                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "Link VARCHAR NOT NULL UNIQUE);";
  char *messaggeError;
  int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), nullptr, nullptr,
                              &messaggeError);

  if (exitCode != SQLITE_OK) {
    sqlite3_free(messaggeError);
  } else {
    spdlog::error("Error performing createDB : {}", messaggeError);
  }
}

bool Database::checkTable() {
  char *messageError;
  std::string sql =
      "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='User' OR name='Invites';";
  int count;
  int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount,
                              &count, &messageError);
  if (exitCode != SQLITE_OK) {
    sqlite3_free(messageError);
    spdlog::error("Error performing checkTable : {}", messageError);
    return false;
  }
  return count == 2;
}

bool Database::openConnection() {
  return sqlite3_open(databaseName.c_str(), &DBConnection) == 0;
}

bool Database::insertUser(User &user) {
  if (openConnection()) {
    std::string sql = "INSERT INTO User VALUES(NULL,?,?,?,?,?,?);";
    QByteArray buffer = convertFromQImage(user.getPicture());
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                      &statement, pzTail);
    if (exitCode == SQLITE_OK) {
      sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 3, user.getName().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 4, user.getSurname().toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 5, user.getEmail().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_blob(statement, 6, buffer.data(), buffer.size(),
                        SQLITE_TRANSIENT);

      exitCode = sqlite3_step(statement);

      if (exitCode == SQLITE_DONE) {
        user = User(user.getUsername(), user.getName(), user.getSurname(),
                    user.getEmail(), user.getPicture());
        sqlite3_finalize(statement);
        sqlite3_close(this->DBConnection);
        return true;
      }
    }
  }
  return false;
}

bool Database::loginUser(User &user) {
  if (openConnection()) {
    std::string sql = "SELECT * FROM User WHERE Username=? AND Password=?;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int ret = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                 &statement, pzTail);
    sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(),
                      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(),
                      -1, SQLITE_TRANSIENT);
    if (ret == SQLITE_OK) {
      ret = sqlite3_step(statement);
      if (ret == SQLITE_ROW) {
        QByteArray buffer = QByteArray::fromRawData(
            reinterpret_cast<const char *>(sqlite3_column_blob(statement, 6)),
            sqlite3_column_bytes(statement, 6));
        user = User(
            reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)),
            reinterpret_cast<const char *>(sqlite3_column_text(statement, 3)),
            reinterpret_cast<const char *>(sqlite3_column_text(statement, 4)),
            reinterpret_cast<const char *>(sqlite3_column_text(statement, 5)),
            convertIntoQImage(buffer));
      }
      sqlite3_finalize(statement);
      sqlite3_close(this->DBConnection);
      return ret == SQLITE_ROW;
    }
  }
  return false;
}

bool Database::updateUser(User &user) {
  if (openConnection()) {
    std::string sql = "UPDATE User set Password = ?, Name= ?, Surname = ?, "
                      "Email = ?, ProfilePic = ? where Username = ?;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    QByteArray buffer = convertFromQImage(user.getPicture());
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                      &statement, pzTail);
    if (exitCode == SQLITE_OK) {

      sqlite3_bind_text(statement, 6, user.getUsername().toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 1, user.getPassword().toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 2, user.getName().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 3, user.getSurname().toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 4, user.getEmail().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_blob(statement, 5, buffer.data(), buffer.size(),
                        SQLITE_TRANSIENT);

      exitCode = sqlite3_step(statement);

      if (exitCode == SQLITE_DONE) {
        user = User(user.getUsername(), user.getName(), user.getSurname(),
                    user.getEmail(), user.getPicture());
        sqlite3_finalize(statement);
        sqlite3_close(this->DBConnection);
        return true;
      }
    }
  }
  return false;
}

bool Database::deleteUser(User user) {
  if (openConnection()) {
    std::string sql = "DELETE FROM User WHERE Username=?;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int ret = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                 &statement, pzTail);
    sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(),
                      -1, SQLITE_TRANSIENT);

    if(ret == SQLITE_OK) {
      ret = sqlite3_step(statement);
      if (ret  == SQLITE_DONE) {
        sqlite3_finalize(statement);
        sqlite3_close(this->DBConnection);
        return true;
      }
    }
  }
  return false;
}

int Database::callbackCount(void *data, int colNum, char **dataRow,
                            char **colName) {
  if (colNum == 1 && dataRow) {
    *static_cast<int *>(data) = std::stoi(dataRow[0]);
    return 0;
  }
  return 1;
}

bool Database::checkInvite(const QString& link) {
  bool isValid = false;
  if (openConnection()) {
    std::string sql = "SELECT COUNT(*) FROM Invites WHERE Link=?;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int ret = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                 &statement, pzTail);
    sqlite3_bind_text(statement, 1, link.toStdString().c_str(),
                      -1, SQLITE_TRANSIENT);

    if(ret == SQLITE_OK) {
      ret = sqlite3_step(statement);
      if (ret  == SQLITE_ROW) {
        isValid = sqlite3_column_int(statement, 0) == 1;
        sqlite3_finalize(statement);
        sqlite3_close(this->DBConnection);
      }
    }
  }
  return isValid;
}
bool Database::insertInvite(const QString &link) {
  if (openConnection()) {
    std::string sql = "INSERT INTO Invites VALUES(NULL,?);";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                      &statement, pzTail);
    if (exitCode == SQLITE_OK) {
      sqlite3_bind_text(statement, 1, link.toStdString().c_str(),
                        -1, SQLITE_TRANSIENT);
      exitCode = sqlite3_step(statement);

      if (exitCode == SQLITE_DONE) {
        sqlite3_finalize(statement);
        sqlite3_close(this->DBConnection);
        return true;
      }
    }
  }
  return false;
}
bool Database::deleteInvite(const QString &link) {
  if (openConnection()) {
    std::string sql = "DELETE FROM Invites WHERE Link=?;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int ret = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                 &statement, pzTail);
    sqlite3_bind_text(statement, 1, link.toStdString().c_str(),
                      -1, SQLITE_TRANSIENT);

    if(ret == SQLITE_OK) {
      ret = sqlite3_step(statement);
      if (ret  == SQLITE_DONE) {
        sqlite3_finalize(statement);
        sqlite3_close(this->DBConnection);
        return true;
      }
    }
  }
  return false;
}


QImage Database::convertIntoQImage(QByteArray buffer) {
  QDataStream ds(&buffer, QIODevice::ReadOnly);
  QImage ret;
  ds >> ret;
  return ret;
}

QByteArray Database::convertFromQImage(const QImage &userPic) {
  QByteArray bytes;
  QDataStream ds(&bytes, QIODevice::WriteOnly);
  ds << userPic;
  return bytes;
}
