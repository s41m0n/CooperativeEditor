#include <filesystem>
#include <algorithm>
#include <QFile>

#include "common/CrdtAlgorithm.h"
#include "Model.h"

Model::Model() : idGenerator(1) {

  //Loading all files name in the directory
  for (auto &p: std::filesystem::directory_iterator(".")) {
    auto filename = p.path().string();
    auto pos = filename.find(".crdt");
    if (pos != std::string::npos) {
      filename.erase(pos, filename.length());
      filename.erase(filename.begin(), filename.begin() + 2);
      availableFiles.push_back(QString::fromStdString(filename));
    }
  }
}

void Model::storeFileSymbols(const std::shared_ptr<ServerFile> &serverFile) {
  QFile file((serverFile->getFileName() + ".crdt"));

  if (!file.open(QIODevice::WriteOnly)) {
    throw std::runtime_error("Unable to open file");
  }

  QDataStream ds(&file);
  ds << serverFile->getFileText();
}


void Model::loadFileSymbols(const std::shared_ptr<ServerFile> &serverFile) {
  QFile file((serverFile->getFileName() + ".crdt"));

  if (!file.open(QIODevice::ReadOnly)) {
    throw std::runtime_error("Unable to load file");
  }

  QDataStream ds(&file);
  ds >> serverFile->getFileText();
}

void Model::userInsert(unsigned int connId, Symbol &symbol) {

  auto serverFile = usersFile[connId];

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::remoteInsert(symbol, serverFile->getFileText());

  storeFileSymbols(serverFile);
}

void Model::userErase(unsigned int connId, Symbol &symbol) {

  auto serverFile = usersFile[connId];

  std::lock_guard<std::mutex> guard(serverFile->mutex);

  CrdtAlgorithm::remoteErase(symbol, serverFile->getFileText());

  storeFileSymbols(serverFile);

}

bool Model::createFileByUser(unsigned connId, const QString &filename) {

  if (std::find(availableFiles.begin(), availableFiles.end(), filename) ==
      availableFiles.end()) {
    return false;
  } else {
    std::lock_guard<std::mutex> guard(usersFileMutex);
    availableFiles.push_back(filename);
    auto newFile = std::make_shared<ServerFile>(filename);
    usersFile[connId] = newFile;
    return true;
  }
}

bool Model::openFileByUser(unsigned connId, QString filename) {

  if (availableFiles.empty() ||
      std::find(availableFiles.begin(), availableFiles.end(), filename) ==
      availableFiles.end()) {
    return false;
  } else {
    auto file = std::find_if(usersFile.begin(), usersFile.end(),
                             [&filename](auto srvFile) {
                                 return srvFile.second->getFileName() ==
                                        filename;
                             });
    std::lock_guard<std::mutex> guard(usersFileMutex);
    if (file == usersFile.end()) {
      auto newFile = std::make_shared<ServerFile>(filename);
      loadFileSymbols(newFile);
      usersFile[connId] = newFile;
    } else {
      usersFile[connId] = file->second;
    }
    return true;
  }
}

QVector<QString> &Model::getAvailableFiles() {
  return availableFiles;
}

FileText &Model::getFileSymbolList(unsigned connId) {
  return usersFile[connId]->getFileText();
}

void Model::removeConnection(unsigned connId) {
  std::lock_guard<std::mutex> lock(usersFileMutex);
  usersFile.erase(connId);
}
