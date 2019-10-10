#include <spdlog/spdlog.h>

#include "Model.h"
#include "common/CrdtAlgorithm.h"

Model::Model() : editorId(0), digitGenerator(0), currentFile() {
}

std::string Model::textify() {
  std::string str;
  std::for_each(symbols.begin(), symbols.end(), [&str](Symbol s) {
      str += s.getChar();
  });
  return str;
}

Symbol *Model::localInsert(int index, char value) {

  symbols.insert(symbols.begin() + index, generateSymbol(index, value));

  return &symbols[index];
}

Symbol *Model::localErase(int index) {
  auto s = (index < symbols.size() && index >= 0) ? &symbols[index] : nullptr;

  if (s != nullptr) {
    symbols.erase(symbols.begin() + index);
  }
  return s;
}

void Model::remoteInsert(Symbol symbol) {
  CrdtAlgorithm::remoteInsert(symbol, symbols);
}

void Model::remoteErase(Symbol symbol) {
  CrdtAlgorithm::remoteErase(symbol, symbols);
}

Symbol Model::generateSymbol(int index, char value) {
  auto pos1 = (index - 1 < symbols.size() && index - 1 >= 0 &&
               !symbols[index - 1].getPos().empty()) ? symbols[
                      index - 1].getPos() : std::vector<Identifier>();
  auto pos2 = (index < symbols.size() && index >= 0 &&
               !symbols[index].getPos().empty()) ? symbols[
                      index].getPos() : std::vector<Identifier>();
  auto newPos = CrdtAlgorithm::generatePosBetween(pos1, pos2, editorId);

  return Symbol(value, editorId, newPos);
}


void Model::setEditorId(unsigned int newEditorId) {
  editorId = newEditorId;
}

unsigned Model::getEditorId() {
  return editorId;
}

void Model::setCurrentFile(std::string &filename) {
  currentFile = filename;
}

void Model::setCurrentFileContent(std::vector<Symbol> &newContent) {
  symbols = std::move(newContent);
}