#include <QFile>
#include <algorithm>
#include <filesystem>

#include "Model.h"
#include "src/include/lib/crdt/CrdtAlgorithm.h"

Model::Model() : idGenerator(1) {

  // Loading all files name in the directory
  for (auto &p :
       std::filesystem::directory_iterator(std::filesystem::current_path())) {
    auto filename = p.path().string();
    auto pos = filename.find(".crdt");
    auto pos2 = filename.rfind(std::filesystem::path::preferred_separator);

    if (pos != std::string::npos) {
      filename.erase(0, pos2 + 1);
      availableFiles.push_back(QString::fromStdString(filename));
    }
  }
}

void Model::storeFileSymbols(const std::shared_ptr<ServerFile> &serverFile) {
  QFile file((serverFile->getFileName()));

  if (!file.open(QIODevice::WriteOnly)) {
    throw std::runtime_error("Unable to open file");
  }

  QDataStream ds(&file);
  ds << serverFile->getFileText();
}

void Model::loadFileSymbols(const std::shared_ptr<ServerFile> &serverFile) {
  QFile file((serverFile->getFileName()));

  if (!file.open(QIODevice::ReadOnly)) {
    throw std::runtime_error("Unable to load file");
  }

  QDataStream ds(&file);
  ds >> serverFile->getFileText();
}

void Model::userInsert(TcpSocket *socket, Symbol symbol) {

  auto serverFile =
      std::find_if(usersFile.begin(), usersFile.end(), [socket](auto &pair) {
        return socket == pair.second;
      })->first;

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::remoteInsert(symbol, serverFile->getFileText());

  storeFileSymbols(serverFile);
}

void Model::userErase(TcpSocket *socket, Symbol symbol) {

  auto serverFile =
      std::find_if(usersFile.begin(), usersFile.end(), [socket](auto &pair) {
        return socket == pair.second;
      })->first;

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::remoteErase(symbol, serverFile->getFileText());

  storeFileSymbols(serverFile);
}

void Model::userReplace(TcpSocket *socket, Symbol symbol) {
  auto serverFile =
          std::find_if(usersFile.begin(), usersFile.end(), [socket](auto &pair) {
              return socket == pair.second;
          })->first;

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::replaceSymbol(symbol, serverFile->getFileText());

  storeFileSymbols(serverFile);
}

bool Model::createFileByUser(TcpSocket *socket, const QString &filename) {

  auto newFile = std::make_shared<ServerFile>(filename + ".crdt");

  if (std::find(availableFiles.begin(), availableFiles.end(),
                newFile->getFileName()) != availableFiles.end()) {
    return false;
  } else {
    storeFileSymbols(newFile);
    std::lock_guard<std::mutex> guard(usersFileMutex);
    availableFiles.push_back(newFile->getFileName());
    usersFile.emplace(newFile, socket);
    return true;
  }
}

bool Model::openFileByUser(TcpSocket *socket, QString filename) {

  if (availableFiles.empty() ||
      std::find(availableFiles.begin(), availableFiles.end(), filename) ==
          availableFiles.end()) {
    return false;
  } else {
    auto file = std::find_if(usersFile.begin(), usersFile.end(),
                             [&filename](auto &srvFile) {
                               return srvFile.first->getFileName() == filename;
                             });
    std::lock_guard<std::mutex> guard(usersFileMutex);
    if (file == usersFile.end()) {
      auto newFile = std::make_shared<ServerFile>(filename);
      loadFileSymbols(newFile);
      usersFile.emplace(newFile, socket);
    } else {
      usersFile.emplace(file->first, socket);
    }
    return true;
  }
}

QVector<QString> &Model::getAvailableFiles() { return availableFiles; }

std::shared_ptr<ServerFile> Model::getFileBySocket(TcpSocket *socket) {
  auto fileToSocket = std::find_if(usersFile.begin(), usersFile.end(),
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

std::vector<TcpSocket *> Model::getFileConnections(const QString &fileName) {
  std::vector<TcpSocket *> fileConnections;
  std::for_each(usersFile.begin(), usersFile.end(), [&](auto &pair) {
    if (pair.first->getFileName() == fileName) {
      fileConnections.push_back(pair.second);
    }
  });
  return fileConnections;
}

void Model::insertUserActivity(TcpSocket *socket, const User& user) {
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
bool Model::checkInvite(const QString &link) {
  return Database::getInstance().checkInvite(link);
}
bool Model::insertInvite(const QString &username, const QString &filename) {
  QString hashedLink = QString(QCryptographicHash::hash(QString(username+filename).toUtf8(), QCryptographicHash::Sha512).toHex());
  return Database::getInstance().insertInvite(hashedLink);
}
bool Model::deleteInvite(const QString &link) {
  return Database::getInstance().deleteInvite(link);
}
