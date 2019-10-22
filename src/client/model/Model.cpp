#include "Model.h"

#include <memory>
#include "common/CrdtAlgorithm.h"

Model::Model() : editorId(0), digitGenerator(0), clientFile(nullptr) {
}

QString Model::textify() {
  QString str;
  for (auto &val : clientFile->getFileText()) {
    str += val.getChar();
  }
  return str;
}

Symbol *Model::localInsert(int index, QChar value) {

  clientFile->getFileText().insert(clientFile->getFileText().begin() + index,
          generateSymbol(index, value));

  return &clientFile->getFileText()[index];
}

Symbol *Model::localErase(int index) {
  auto s = (index < clientFile->getFileText().size() && index >= 0) ?
          &clientFile->getFileText()[index] : nullptr;

  if (s != nullptr) {
    clientFile->getFileText().erase(clientFile->getFileText().begin() + index);
  }
  return s;
}

void Model::remoteInsert(Symbol symbol) {
  CrdtAlgorithm::remoteInsert(symbol, clientFile->getFileText());
}

void Model::remoteErase(Symbol symbol) {
  CrdtAlgorithm::remoteErase(symbol, clientFile->getFileText());
}

Symbol Model::generateSymbol(int index, QChar value) {
  auto pos1 = (index - 1 < clientFile->getFileText().size() && index - 1 >= 0 &&
               !clientFile->getFileText()[index - 1].getPos().empty()) ?
                       clientFile->getFileText()[
                      index - 1].getPos() : QVector<Identifier>();
  auto pos2 = (index < clientFile->getFileText().size() && index >= 0 &&
               !clientFile->getFileText()[index].getPos().empty()) ?
                       clientFile->getFileText()[
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

void Model::setCurrentFile(const QString &filename) {
  clientFile = std::make_unique<ClientFile>(filename);
}

void Model::setCurrentFileContent(FileText &newContent) {
  clientFile->getFileText() = std::move(newContent);
}