#include "Model.h"

#include <memory>

#include "src/include/lib/crdt/CrdtAlgorithm.h"

Model::Model() : editorId(0), digitGenerator(0), file(), user() {
}

User Model::getUser() {
  return this->user;
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

Symbol &Model::localInsert(int index, QChar value) {

  if(index > file.getFileText().size() || index < 0) {
    throw std::runtime_error("No valid position: TextSize:" + std::to_string(file.getFileText().size()));
  }

  file.getFileText().insert(file.getFileText().begin() + index,
                            generateSymbol(index, value));

  return file.getFileText()[index];
}

Symbol Model::localErase(int index) {

  if(index >= file.getFileText().size() || index < 0) {
    throw std::runtime_error("No symbol to erase: TextSize:" + std::to_string(file.getFileText().size()));
  }

  Symbol s = std::move(file.getFileText()[index]);
  file.getFileText().erase(file.getFileText().begin() + index);

  return std::move(s);
}

Symbol &Model::localUpdate(int index, bool attributes[Attribute::ATTRIBUTE_SIZE]) {
  if(index >= file.getFileText().size() || index < 0) {
    throw std::runtime_error("No symbol to update: TextSize:" + std::to_string(file.getFileText().size()));
  }

  Symbol &s = file.getFileText()[index];
  s.setAttributes(attributes);

  return s;
}

void Model::remoteInsert(Symbol &symbol) {
  CrdtAlgorithm::remoteInsert(symbol, file.getFileText());
}

void Model::remoteErase(Symbol &symbol) {
  CrdtAlgorithm::remoteErase(symbol, file.getFileText());
}

void Model::remoteUpdate(Symbol &symbol) {
  CrdtAlgorithm::replaceSymbol(symbol, file.getFileText());
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

  return std::move(Symbol(value, editorId, newPos));
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
