//
// Created by s41m0n on 10/06/19.
//

#include <fstream>
#include <spdlog/spdlog.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Model.h"
#include "src/utility/CrdtAlgorithm.h"

Model::Model() : editorId(-1), digitGenerator(0), currentFile() {
  spdlog::debug("SharedEditor::Created Model");
}

Model::~Model() {
  spdlog::debug("SharedEditor::Destroyed Model");
}

void Model::writeOnFile() {
  std::ofstream outfile;
  std::string filename = "SE" + std::to_string(editorId) + currentFile ;
  outfile.open(filename);
  outfile << to_string() << std::endl;
}

const std::string Model::to_string() {
  std::string str;
  std::for_each(symbols.begin(), symbols.end(), [&str](Symbol s){
      str += s.getChar();
  });
  return str;
}

const Symbol* Model::localInsert(int index, char value) {

  symbols.insert(symbols.begin() + index, generateSymbol(index, value));

  writeOnFile();

  return &symbols[index];
}

const Symbol* Model::localErase(int index) {
  auto s = (index < symbols.size() && index >= 0) ? &symbols[index] : nullptr;

  if (s != nullptr) {

    symbols.erase(symbols.begin() + index);

    writeOnFile();
  }
  return s;
}

void Model::remoteInsert(Symbol symbol) {
  CrdtAlgorithm::remoteInsert(std::move(symbol), symbols);
}

void Model::remoteErase(Symbol symbol) {
  CrdtAlgorithm::remoteErase(std::move(symbol), symbols);
}

const Symbol Model::generateSymbol(int index, char value) {
  auto symbolBefore = (index-1 < symbols.size() && index - 1 >= 0 && !symbols[index - 1].getPos().empty()) ? &symbols[index - 1] : nullptr;
  auto symbolAfter = (index < symbols.size() && index >= 0 && !symbols[index].getPos().empty()) ? &symbols[index] : nullptr;
  std::vector<int> newPos;
  CrdtAlgorithm::generatePosBetween(symbolBefore, symbolAfter, &newPos);
  Symbol s(value, editorId, digitGenerator++, newPos);
  return s;
}

const int Model::getEditorId() {
  return editorId;
}

void Model::setEditorId(int newEditorId) {
  editorId = newEditorId;
}

void Model::setCurrentFile(std::string &filename) {
  currentFile = filename;
}

void Model::setCurrentFileContent(std::vector<Symbol> newContent) {
  symbols = std::move(newContent);
}