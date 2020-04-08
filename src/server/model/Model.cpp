#include "Model.h"

Model::Model() : idGenerator(1) {}

void Model::userInsert(TcpSocket *socket, Symbol &symbol) {

  auto serverFile =
      std::find_if(usersFile.begin(), usersFile.end(), [socket](auto &pair) {
        return socket == pair.second;
      })->first;

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::remoteInsert(symbol, serverFile->getFileText());

  Database::getInstance().updateFile(*serverFile);
}

void Model::userErase(TcpSocket *socket, Symbol &symbol) {

  auto serverFile =
      std::find_if(usersFile.begin(), usersFile.end(), [socket](auto &pair) {
        return socket == pair.second;
      })->first;

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::remoteErase(symbol, serverFile->getFileText());

  Database::getInstance().updateFile(*serverFile);
}

bool Model::createFileByUser(TcpSocket *socket, const QString &filename) {

  auto fileID =
      Database::getInstance().getUserFileID(filename, socketToUser[socket]);
  if (fileID > 0)
    return false;
  std::shared_ptr<ServerFile> newFile;
  if (Database::getInstance().insertFile(socketToUser[socket], filename,
                                         newFile)) {
    std::lock_guard<std::mutex> guard(usersFileMutex);
    usersFile.emplace(newFile, socket);
    return true;
  }
  return false;
}

bool Model::openFileByUser(TcpSocket *socket, const QString &filename) {

  auto fileID =
      Database::getInstance().getUserFileID(filename, socketToUser[socket]);
  auto file = std::find_if(usersFile.begin(), usersFile.end(),
                           [&fileID](auto &srvFile) {
                             return srvFile.first->getFileID() == fileID;
                           });

  if (file != usersFile.end()) {
    std::lock_guard<std::mutex> guard(usersFileMutex);
    usersFile.emplace(file->first, socket);
  } else {
    std::shared_ptr<ServerFile> newFile;
    if (!Database::getInstance().openFile(fileID, filename, newFile)) {
      return false;
    }
    std::lock_guard<std::mutex> guard(usersFileMutex);
    usersFile.emplace(newFile, socket);
  }
  return true;
}

QVector<QString> Model::getAvailableUserFiles(User &user) {
  return Database::getInstance().getUserFiles(user);
}

std::shared_ptr<ServerFile> Model::getFileBySocket(TcpSocket *socket) {
  auto fileToSocket =
      std::find_if(usersFile.begin(), usersFile.end(),
                   [socket](auto &pair) { return socket == pair.second; });
  if (fileToSocket != usersFile.end())
    return fileToSocket->first;
  return nullptr;
}

void Model::removeConnection(TcpSocket *socket) {
  std::lock_guard<std::mutex> lock(usersFileMutex);
  auto toErase =
      std::find_if(usersFile.begin(), usersFile.end(),
                   [socket](auto &pair) { return socket == pair.second; });
  if (toErase != usersFile.end()) {
    usersFile.erase(toErase);
  }
}

std::vector<TcpSocket *> Model::getFileConnections(int fileID) {
  std::vector<TcpSocket *> fileConnections;
  std::for_each(usersFile.begin(), usersFile.end(), [&](auto &pair) {
    if (pair.first->getFileID() == fileID) {
      fileConnections.push_back(pair.second);
    }
  });
  return fileConnections;
}

void Model::insertUserActivity(TcpSocket *socket, const User &user) {
  socketToUser.emplace(socket, user);
}

void Model::removeUserActivity(TcpSocket *socket) {
  if (socketToUser.find(socket) != socketToUser.end())
    socketToUser.erase(socket);
}

const User &Model::getUserActivity(TcpSocket *socket) {
  return socketToUser.find(socket)->second;
}

bool Model::logInUser(User &user) {
  return Database::getInstance().loginUser(user);
}

bool Model::registerUser(User &user) {
  return Database::getInstance().insertUser(user);
}

bool Model::updateUser(User &user) {
  return Database::getInstance().updateUser(user);
}
bool Model::deleteUser(User &user) {
  return Database::getInstance().deleteUser(user);
}

QString Model::generateInvite(TcpSocket *sender, const QString &filename) {
  QString code = QString(socketToUser[sender].getUsername() + "/" + filename)
                     .toUtf8()
                     .toBase64();
  Database::getInstance().insertInvite(code);
  return code;
}
bool Model::insertInviteCode(TcpSocket *sender, const QString &code,
                             File &file) {
  auto decoded = QString(QByteArray::fromBase64(code.toUtf8())).split("/");
  if (decoded.size() == 2 &&
      Database::getInstance().checkInvite(code, decoded[0], decoded[1],
                                          socketToUser[sender].getUsername()) &&
      openFileByUser(sender, decoded[1])) {
    file.getFileName() = decoded[1];
    file.getFileText() = getFileBySocket(sender)->getFileText();
    return true;
  }
  return false;
}
