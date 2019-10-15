#include <spdlog/spdlog.h>

#include "Model.h"
#include "common/CrdtAlgorithm.h"

Model::Model() : editorId(0), digitGenerator(0), currentFile() {
}

QString Model::textify() {
  QString str;
  for (auto &val : symbols) {
    str += val.getChar();
  }
  return str;
}

Symbol *Model::localInsert(int index, QChar value) {

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

Symbol Model::generateSymbol(int index, QChar value) {
  auto pos1 = (index - 1 < symbols.size() && index - 1 >= 0 &&
               !symbols[index - 1].getPos().empty()) ? symbols[
                      index - 1].getPos() : QVector<Identifier>();
  auto pos2 = (index < symbols.size() && index >= 0 &&
               !symbols[index].getPos().empty()) ? symbols[
                      index].getPos() : QVector<Identifier>();
  auto newPos = CrdtAlgorithm::generatePosBetween(pos1, pos2, editorId);

  return Symbol(value, editorId, newPos);
}


void Model::setEditorId(unsigned int newEditorId) {
  editorId = newEditorId;
}

unsigned Model::getEditorId() {
  return editorId;
}

std::string Model::textifyToStdString() {
  return textify().toStdString();
}

void Model::setCurrentFile(QString &filename) {
  currentFile = filename;
  symbols.clear();
}

void Model::setCurrentFileContent(QVector<Symbol> &newContent) {
  symbols = std::move(newContent);
}