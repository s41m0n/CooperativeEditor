//
// Created by s41m0n on 07/06/19.
//

#include <spdlog/spdlog.h>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Model.h"
#include "crdtAlgorithm/CrdtAlgorithm.h"

Model::Model() : idGenerator(0){

  spdlog::debug("NetworkServer::Created Model");

  //Loading all files name in the directory
  boost::filesystem::recursive_directory_iterator it(boost::filesystem::current_path());
  boost::filesystem::recursive_directory_iterator end;

  while(it != end) {

    if(boost::filesystem::is_regular_file(*it) && it->path().extension() == ".crdt") {
      std::string toAdd = it->path().filename().string();
      size_t index = toAdd.find_last_of('.');
      toAdd = toAdd.substr(0, index);
      availableFiles.append(toAdd + ";");
    }
    ++it;
  }
}

Model::~Model() {

  spdlog::debug("NetworkServer::Destroyed Model");

}

void Model::storeFileSymbols(std::string& filename) {
  std::ofstream file{filename + ".crdt"};
  boost::archive::text_oarchive oa{file};
  oa << openedFiles[filename];
}

void Model::loadFileSymbols(std::string& filename) {
  std::ifstream file{filename + ".crdt"};
  boost::archive::text_iarchive oa{file};
  oa >> openedFiles[filename];
}

unsigned int Model::generateEditorId() {
  return idGenerator++;
}

void Model::userInsert(int connId, Symbol symbol) {

  auto filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteInsert(std::move(symbol), openedFiles[filename]);

  storeFileSymbols(filename);
}

void Model::userErase(int connId, Symbol symbol) {

  std::string filename = usersFile[connId];

  std::lock_guard<std::mutex> guard(*openedFilesMutexes[filename]);

  CrdtAlgorithm::remoteErase(std::move(symbol), openedFiles[filename]);

  storeFileSymbols(filename);

}

bool Model::createFileByUser(int connId, std::string& filename) {
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

bool Model::openFileByUser(int connId, std::string& filename) {
  std::lock_guard<std::mutex> guard(openedFilesMapMutex);

  if(availableFiles.empty() || availableFiles.find(filename))
    return false;
  else {
    usersFile[connId] = filename;
    if(openedFiles.find(filename) == openedFiles.end()) {
      openedFiles.emplace(filename, std::vector<Symbol>());
      openedFilesMutexes.emplace(filename, std::make_unique<std::mutex>());
      loadFileSymbols(filename);
    }
    return true;
  }
}

const std::string Model::getAvailableFiles() {
  return availableFiles;
}

const std::vector<Symbol> Model::getFileSymbolList(int connId) {
  return openedFiles[usersFile[connId]];
}