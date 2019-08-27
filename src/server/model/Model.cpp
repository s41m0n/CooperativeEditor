//
// Created by s41m0n on 07/06/19.
//

#include <spdlog/spdlog.h>
#include <filesystem>
#include <algorithm>
#include <QFile>

#include "Model.h"
#include "src/utility/CrdtAlgorithm.h"

Model::Model() : idGenerator(1){

  spdlog::debug("Created Model");

  //Loading all files name in the directory
  for(auto& p: std::filesystem::directory_iterator(".")) {
    auto filename = p.path().string();
    auto pos = filename.find(".crdt");
    if (pos != std::string::npos) {
      filename.erase(pos, filename.length());
      filename.erase(filename.begin(), filename.begin() + 2);
      availableFiles += filename + ";";
    }
  }
}

Model::~Model() {

  spdlog::debug("Destroyed Model");

}


void Model::storeFileSymbols(std::string& filename) {
  QFile file((filename + ".crdt").c_str());

  if(!file.open(QIODevice::WriteOnly)) {
    throw std::runtime_error("Unable to open file");
  }

  QDataStream ds(&file);
  ds << openedFiles[filename];
}


void Model::loadFileSymbols(std::string& filename) {
  QFile file((filename + ".crdt").c_str());

  if(!file.open(QIODevice::ReadOnly)) {
    throw std::runtime_error("Unable to load file");
  }

  QDataStream ds(&file);
  ds >> openedFiles[filename];
}

unsigned int Model::generateEditorId() {
  return idGenerator++;
}

void Model::userInsert(unsigned int connId, Symbol symbol) {

  auto filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteInsert(std::move(symbol), openedFiles[filename]);

  storeFileSymbols(filename);
}

void Model::userErase(unsigned int connId, Symbol symbol) {

  std::string filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteErase(std::move(symbol), openedFiles[filename]);

  storeFileSymbols(filename);

}

bool Model::createFileByUser(unsigned int connId, std::string& filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if (availableFiles.find(filename) != std::string::npos) {
    return false;
  } else {
    availableFiles.append(filename + ";");
    usersFile[connId] = filename;
    openedFiles.emplace(filename, std::vector<Symbol>());
    openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
    return true;
  }
}

bool Model::openFileByUser(unsigned int connId, std::string& filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if(availableFiles.empty() || availableFiles.find(filename)) {
    return false;
  } else {
    usersFile[connId] = filename;
    if(openedFiles.find(filename) == openedFiles.end()) {
      openedFiles.emplace(filename, std::vector<Symbol>());
      openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
      loadFileSymbols(filename);
    }
    return true;
  }
}

std::string& Model::getAvailableFiles() {
  return availableFiles;
}

std::vector<Symbol> Model::getFileSymbolList(unsigned int connId) {
  return openedFiles[usersFile[connId]];
}