#include <spdlog/spdlog.h>
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
      availableFiles.emplace_back(filename);
    }
  }

}

void Model::storeFileSymbols(std::string &filename) {
  QFile file((filename + ".crdt").c_str());

  if (!file.open(QIODevice::WriteOnly)) {
    throw std::runtime_error("Unable to open file");
  }

  QDataStream ds(&file);
  ds << openedFiles[filename];
}


void Model::loadFileSymbols(std::string &filename) {
  QFile file((filename + ".crdt").c_str());

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

  std::string filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteErase(symbol, openedFiles[filename]);

  storeFileSymbols(filename);

}

bool Model::createFileByUser(unsigned int connId, const std::string &filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if (std::find(availableFiles.begin(), availableFiles.end(), filename) ==
      availableFiles.end()) {
    return false;
  } else {
    availableFiles.emplace_back(filename);
    usersFile[connId] = filename;
    openedFiles.emplace(filename, std::vector<Symbol>());
    openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
    return true;
  }
}

bool Model::openFileByUser(unsigned int connId, std::string filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if (availableFiles.empty() ||
      std::find(availableFiles.begin(), availableFiles.end(), filename) ==
      availableFiles.end()) {
    return false;
  } else {
    usersFile[connId] = filename;
    if (openedFiles.find(filename) == openedFiles.end()) {
      openedFiles.emplace(filename, std::vector<Symbol>());
      openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
      loadFileSymbols(filename);
    }
    return true;
  }
}

std::vector<std::string> &Model::getAvailableFiles() {
  return availableFiles;
}

std::vector<Symbol> &Model::getFileSymbolList(unsigned int connId) {
  return openedFiles[usersFile[connId]];
}