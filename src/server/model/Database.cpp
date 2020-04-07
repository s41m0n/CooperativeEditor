#include "Database.h"

Database::Database() {
  if (!openConnection()) {
    printErrorAndExit();
  }

  if (!checkTable()) {
    createDB();
  }
  sqlite3_close(this->DBConnection);
}

void Database::printErrorAndExit(const std::string& error) {
  spdlog::error("{}", error);
  exit(-1);
}

void Database::createDB() {
  std::string sql = "CREATE TABLE User ("
                    "Username VARCHAR PRIMARY KEY NOT NULL UNIQUE,"
                    "Password VARCHAR NOT NULL,"
                    "Name VARCHAR,"
                    "Surname VARCHAR,"
                    "Email VARCHAR NOT NULL UNIQUE,"
                    "ProfilePic BLOB);"

                    "CREATE TABLE File ("
                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "Name VARCHAR NOT NULL,"
                    "Content BLOB);"

                    "CREATE TABLE UserFile ("
                    "Username VARCHAR NOT NULL,"
                    "FileID INTEGER,"
                    "PRIMARY KEY (Username, FileID),"
                    "FOREIGN KEY(Username) REFERENCES User(Username),"
                    "FOREIGN KEY(FileID) REFERENCES File(ID));"

                    "CREATE TABLE Invites ("
                    "Link VARCHAR PRIMARY KEY NOT NULL UNIQUE);";
  char *messaggeError;
  int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), nullptr, nullptr,
                              &messaggeError);

  if (exitCode != SQLITE_OK) {
    printErrorAndExit("Error performing createDB : " +
                      std::string(messaggeError));
  }
}

bool Database::checkTable() {
  char *messageError;
  std::string sql =
      "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='User' "
      "OR name='Invites' OR name='File' OR name='UserFile';";
  int count;
  int exitCode = sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount,
                              &count, &messageError);
  if (exitCode != SQLITE_OK) {
    printErrorAndExit("Error performing checkTable : " +
                      std::string(messageError));
  }
  return count == 4;
}

bool Database::openConnection() {
  return sqlite3_open(databaseName.c_str(), &DBConnection) == 0;
}

bool Database::insertUser(User &user) {
  if (!openConnection())
    printErrorAndExit();

  std::string sql = "INSERT INTO User VALUES(?,?,?,?,?,?);";
  QByteArray buffer = convertFromQImage(user.getPicture());
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }

  sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, user.getName().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 4, user.getSurname().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 5, user.getEmail().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_blob(statement, 6, buffer.data(), buffer.size(),
                    SQLITE_TRANSIENT);

  if (sqlite3_step(statement) != SQLITE_DONE) {
    /*Username already taken of parameter hacked*/
    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return false;
  }
  user = User(user.getUsername(), user.getName(), user.getSurname(),
              user.getEmail(), user.getPicture());
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return true;
}

bool Database::loginUser(User &user) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "SELECT Username, Name, Surname, Email FROM User WHERE "
                    "Username=? AND Password=?;";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }
  sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, user.getPassword().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  if (sqlite3_step(statement) != SQLITE_ROW) {
    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return false;
  }
  QByteArray buffer = QByteArray::fromRawData(
      reinterpret_cast<const char *>(sqlite3_column_blob(statement, 6)),
      sqlite3_column_bytes(statement, 6));
  user = User(reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)),
              reinterpret_cast<const char *>(sqlite3_column_text(statement, 1)),
              reinterpret_cast<const char *>(sqlite3_column_text(statement, 2)),
              reinterpret_cast<const char *>(sqlite3_column_text(statement, 3)),
              convertIntoQImage(buffer));

  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return true;
}

bool Database::updateUser(User &user) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "UPDATE User set Password = ?, Name= ?, Surname = ?, "
                    "Email = ?, ProfilePic = ? where Username = ?;";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }

  QByteArray buffer = convertFromQImage(user.getPicture());

  sqlite3_bind_text(statement, 6, user.getUsername().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 1, user.getPassword().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, user.getName().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, user.getSurname().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 4, user.getEmail().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_blob(statement, 5, buffer.data(), buffer.size(),
                    SQLITE_TRANSIENT);

  if (sqlite3_step(statement) != SQLITE_DONE) {
    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return false;
  }
  user = User(user.getUsername(), user.getName(), user.getSurname(),
              user.getEmail(), user.getPicture());
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return true;
}

bool Database::deleteUser(User user) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "DELETE FROM User WHERE Username=?;";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }

  sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);

  bool ret = true;
  if (sqlite3_step(statement) != SQLITE_DONE) {
    ret = false;
  }
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return ret;
}

int Database::callbackCount(void *data, int colNum, char **dataRow,
                            char **colName) {
  if (colNum == 1 && dataRow) {
    *static_cast<int *>(data) = std::stoi(dataRow[0]);
    return 0;
  }
  return 1;
}

QVector<QString> Database::getUserFiles(User &user) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "SELECT Name FROM File WHERE ID IN (SELECT FileID FROM "
                    "UserFile WHERE Username=?)";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }
  sqlite3_bind_text(statement, 1, user.getUsername().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);

  QVector<QString> ret;
  while (sqlite3_step(statement) == SQLITE_ROW) {
    ret.append(
        reinterpret_cast<const char *>(sqlite3_column_text(statement, 0)));
  }
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return ret;
}

bool Database::insertFile(User &user, const QString &file,
                          std::shared_ptr<ServerFile> &serverFile) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "INSERT INTO File VALUES(NULL,?,NULL);";
  int id;
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }
  sqlite3_bind_text(statement, 1, file.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);

  if (sqlite3_step(statement) != SQLITE_DONE) {
    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return false;
  }
  sqlite3_finalize(statement);
  sql = "SELECT last_insert_rowid();";
  if (sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount, &id,
                   nullptr) != SQLITE_OK) {
    sqlite3_close(this->DBConnection);
    return false;
  }
  sql = "INSERT INTO UserFile VALUES(\"" + user.getUsername().toStdString() +
        "\"," + std::to_string(id) + ")";
  if (sqlite3_exec(this->DBConnection, sql.c_str(), callbackCount, &id,
                   nullptr) != SQLITE_OK) {
    sqlite3_close(this->DBConnection);
    return false;
  }
  serverFile = std::make_shared<ServerFile>(id, file, FileText());
  sqlite3_close(this->DBConnection);
  return true;
}

int Database::getUserFileID(const QString &filename, User &user) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "SELECT ID FROM File WHERE Name = ? AND ID IN (SELECT "
                    "FileID FROM UserFile WHERE Username=?)";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }

  sqlite3_bind_text(statement, 1, filename.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, user.getUsername().toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);

  int id = -1;
  if (sqlite3_step(statement) == SQLITE_ROW) {
    id = sqlite3_column_int(statement, 0);
  }
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return id;
}

bool Database::openFile(int fileID, const QString &name,
                        std::shared_ptr<ServerFile> &serverFile) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "SELECT Content FROM File WHERE ID = ?";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }

  sqlite3_bind_int(statement, 1, fileID);

  if (sqlite3_step(statement) != SQLITE_ROW) {
    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return false;
  }

  QByteArray buffer = QByteArray::fromRawData(
      reinterpret_cast<const char *>(sqlite3_column_blob(statement, 0)),
      sqlite3_column_bytes(statement, 0));

  serverFile =
      std::make_shared<ServerFile>(fileID, name, convertIntoFileText(buffer));
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return true;
}

bool Database::updateFile(ServerFile &file) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "UPDATE File set Content = ? WHERE ID = ?;";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }
  QByteArray buffer = convertFromFileText(file.getFileText());

  sqlite3_bind_blob(statement, 1, buffer.data(), buffer.size(),
                    SQLITE_TRANSIENT);
  sqlite3_bind_int(statement, 2, file.getFileID());

  bool ret = true;
  if (sqlite3_step(statement) != SQLITE_DONE) {
    ret = false;
  }
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return ret;
}

bool Database::checkInvite(const QString &link, QString &userInviting,
                           QString &filename, QString &userInvited) {
  if (!openConnection()) {
    printErrorAndExit();
  }
  std::string sql = "SELECT COUNT(*) FROM Invites WHERE Link=?;";
  sqlite3_stmt *statement = nullptr;

  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }
  sqlite3_bind_text(statement, 1, link.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);

  if (sqlite3_step(statement) != SQLITE_ROW ||
      sqlite3_column_int(statement, 0) != 1) {
    sqlite3_finalize(statement);
    sqlite3_close(this->DBConnection);
    return false;
  }
  sqlite3_finalize(statement);
  sql = "INSERT INTO UserFile VALUES(?,(SELECT ID FROM File WHERE Name=? "
        "AND ID IN (SELECT FileID FROM UserFile WHERE Username=?))) ";
  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }
  sqlite3_bind_text(statement, 1, userInvited.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 2, filename.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  sqlite3_bind_text(statement, 3, userInviting.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);
  bool ret = true;
  if (sqlite3_step(statement) != SQLITE_DONE) {
    ret = false;
  }
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return ret;
}

bool Database::insertInvite(const QString &link) {
  if (!openConnection()) {
    printErrorAndExit();
  }

  std::string sql = "INSERT INTO Invites VALUES(?);";
  sqlite3_stmt *statement = nullptr;
  if (sqlite3_prepare_v2(this->DBConnection, sql.c_str(), -1, &statement,
                         nullptr) != SQLITE_OK) {
    printErrorAndExit("Failed to prepare statement");
  }

  sqlite3_bind_text(statement, 1, link.toStdString().c_str(), -1,
                    SQLITE_TRANSIENT);

  bool ret = true;
  if (sqlite3_step(statement) != SQLITE_DONE) {
    ret = false;
  }
  sqlite3_finalize(statement);
  sqlite3_close(this->DBConnection);
  return ret;
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
QByteArray Database::convertFromFileText(FileText &text) {
  QByteArray buffer;
  QDataStream ds(&buffer, QIODevice::WriteOnly);
  ds << text;
  return buffer;
}

FileText Database::convertIntoFileText(QByteArray buffer) {
  QDataStream ds(&buffer, QIODevice::ReadOnly);
  FileText text;
  ds >> text;
  return text;
}