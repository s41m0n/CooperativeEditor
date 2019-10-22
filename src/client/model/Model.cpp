#include "Model.h"

#include <memory>
#include "common/CrdtAlgorithm.h"

Model::Model() : editorId(0), digitGenerator(0), file(), user() {
}

QString Model::textify() {
  QString str;
  for (auto &val : file.getFileText()) {
    str += val.getChar();
  }
  return str;
}

std::string Model::textifyToStdString() {
  return textify().toStdString();
}

Symbol *Model::localInsert(int index, QChar value) {

  file.getFileText().insert(file.getFileText().begin() + index,
                            generateSymbol(index, value));

  return &file.getFileText()[index];
}

Symbol *Model::localErase(int index) {
  auto s = (index < file.getFileText().size() && index >= 0) ?
           &file.getFileText()[index] : nullptr;

  if (s != nullptr) {
    file.getFileText().erase(file.getFileText().begin() + index);
  }
  return s;
}

void Model::remoteInsert(Symbol symbol) {
  CrdtAlgorithm::remoteInsert(symbol, file.getFileText());
}

void Model::remoteErase(Symbol symbol) {
  CrdtAlgorithm::remoteErase(symbol, file.getFileText());
}

Symbol Model::generateSymbol(int index, QChar value) {
  auto pos1 = (index - 1 < file.getFileText().size() && index - 1 >= 0 &&
               !file.getFileText()[index - 1].getPos().empty()) ?
              file.getFileText()[
                      index - 1].getPos() : QVector<Identifier>();
  auto pos2 = (index < file.getFileText().size() && index >= 0 &&
               !file.getFileText()[index].getPos().empty()) ?
              file.getFileText()[
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

void Model::setCurrentFile(File &fileToSet) {
  file = fileToSet;
}

void Model::setCurrentUser(User &userToSet) {
  user = std::move(userToSet);
}
