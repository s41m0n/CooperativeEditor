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

void Model::storeFileSymbols(QString &filename) {
  QFile file((filename + ".crdt"));

  if (!file.open(QIODevice::WriteOnly)) {
    throw std::runtime_error("Unable to open file");
  }

  QDataStream ds(&file);
  ds << openedFiles[filename];
}


void Model::loadFileSymbols(QString &filename) {
  QFile file((filename + ".crdt"));

  if (!file.open(QIODevice::ReadOnly)) {
    throw std::runtime_error("Unable to load file");
  }

  QDataStream ds(&file);
  ds >> openedFiles[filename];
}

unsigned int Model::generateEditorId() {
  return idGenerator++;
}

void Model::userInsert(unsigned int connId, Symbol &symbol) {

  auto filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteInsert(symbol, openedFiles[filename]);

  storeFileSymbols(filename);
}

void Model::userErase(unsigned int connId, Symbol &symbol) {

  auto filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteErase(symbol, openedFiles[filename]);

  storeFileSymbols(filename);

}

bool Model::createFileByUser(unsigned connId, const QString &filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if (std::find(availableFiles.begin(), availableFiles.end(), filename) ==
      availableFiles.end()) {
    return false;
  } else {
    availableFiles.push_back(filename);
    usersFile[connId] = filename;
    openedFiles.emplace(filename, QVector<Symbol>());
    openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
    return true;
  }
}

bool Model::openFileByUser(unsigned connId, QString filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if (availableFiles.empty() ||
      std::find(availableFiles.begin(), availableFiles.end(), filename) ==
      availableFiles.end()) {
    return false;
  } else {
    usersFile[connId] = filename;
    if (openedFiles.find(filename) == openedFiles.end()) {
      openedFiles.emplace(filename, QVector<Symbol>());
      openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
      loadFileSymbols(filename);
    }
    return true;
  }
}

QVector<QString> &Model::getAvailableFiles() {
  return availableFiles;
}

QVector<Symbol> &Model::getFileSymbolList(unsigned connId) {
  return openedFiles[usersFile[connId]];
}