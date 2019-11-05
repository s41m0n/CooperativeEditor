#include "Database.h"
#include <spdlog/spdlog.h>

Database::Database() {

  if (openConnection()) {
    char *messaggeError;
    std::string sql = "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='User';";
    int count;
    int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount,
                                &count, &messaggeError);
    if (exitCode != SQLITE_OK || count != 1) {

      sqlite3_free(messaggeError);
      sql = "CREATE TABLE User ("
            "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "Username VARCHAR NOT NULL UNIQUE,"
            "Password VARCHAR NOT NULL,"
            "Name VARCHAR,"
            "Surname VARCHAR,"
            "Email VARCHAR NOT NULL UNIQUE,"
            "PhotoPath     VARCHAR);";
      exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), nullptr,
                              nullptr,
                              &messaggeError);

      if (exitCode != SQLITE_OK) {
        spdlog::error("Sqlite3: {}", messaggeError);
        sqlite3_free(messaggeError);
        exit(-1);
      }
    }

    sqlite3_close(this->DBConnection);
  }
}

int Database::openConnection() {
  return sqlite3_open(databaseName.c_str(), &DBConnection) == 0;
}

bool Database::insertUser(User &user) {
  if (openConnection()) {
    std::string sql = "INSERT INTO User VALUES(NULL,?,?,?,?,?,'path');";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                      &statement, pzTail);
    if (exitCode == SQLITE_OK) {

      sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 3, user.getName().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 4, user.getSurname().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 5, user.getEmail().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);

      exitCode = sqlite3_step(statement);

      if (exitCode == SQLITE_DONE) {
        user = User(user.getIcon(), user.getUsername(), user.getName(),
                    user.getSurname(), user.getEmail());
      }

      sqlite3_finalize(statement);
      sqlite3_close(this->DBConnection);
      return exitCode == SQLITE_DONE;
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
                      -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(),
                      -1,
                      SQLITE_TRANSIENT);
    if (ret == SQLITE_OK) {

      ret = sqlite3_step(statement);

      if (ret == SQLITE_ROW) {
        user = User(
                reinterpret_cast<const char *>(sqlite3_column_text(statement,
                                                                   6)),
                reinterpret_cast<const char *>(sqlite3_column_text(statement,
                                                                   1)),
                reinterpret_cast<const char *>(sqlite3_column_text(statement,
                                                                   3)),
                reinterpret_cast<const char *>(sqlite3_column_text(statement,
                                                                   4)),
                reinterpret_cast<const char *>(sqlite3_column_text(statement,
                                                                   5)));
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
    std::string sql = "UPDATE User set Password = ?, Name= ?, Surname = ?, Email = ? where Username = ?;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                      &statement, pzTail);
    if (exitCode == SQLITE_OK) {

      sqlite3_bind_text(statement, 5, user.getUsername().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 1, user.getPassword().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 2, user.getName().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 3, user.getSurname().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_text(statement, 4, user.getEmail().toStdString().c_str(), -1,
                        SQLITE_TRANSIENT);

      exitCode = sqlite3_step(statement);

      if (exitCode == SQLITE_DONE) {

        user = User(user.getIcon(), user.getUsername(), user.getName(),
                    user.getSurname(), user.getEmail());
      }

      sqlite3_finalize(statement);
      sqlite3_close(this->DBConnection);
      return exitCode == SQLITE_DONE;
    }
  }
  return false;
}

bool Database::deleteUser(User user) {
  if (openConnection()) {
    std::string sql = "DELETE FROM 'User' WHERE username=? ;";
    const char *zTail = nullptr;
    const char **pzTail = &zTail;
    sqlite3_stmt *statement = nullptr;
    int exitCode = sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1,
                                      &statement, pzTail);
    if (exitCode == SQLITE_OK) {

      sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(),
                        -1,
                        SQLITE_TRANSIENT);

      exitCode = sqlite3_step(statement);

      sqlite3_finalize(statement);
      sqlite3_close(this->DBConnection);
      return exitCode == SQLITE_DONE;
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

