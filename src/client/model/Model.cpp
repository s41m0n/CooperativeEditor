#include "Model.h"

#include <utility>

Model::Model() : editorId(0), file(), user() {}

User &Model::getUser() { return this->user; }

std::string Model::toStdString() {
  std::string str;
  for (auto &val : file.getFileText()) {
    str += val.getChar().toLatin1();
  }
  return str;
}

Symbol Model::localInsert(int index, QChar value, QTextCharFormat &format) {

  if (index > file.getFileText().size() || index < 0) {
    throw std::runtime_error("No valid position: TextSize:" +
                             std::to_string(file.getFileText().size()));
  }

  Symbol s = generateSymbol(index, value);
  s.setFormat(format);
  file.getFileText().insert(file.getFileText().begin() + index, s);

  return s;
}

Symbol Model::localErase(int index) {

  if (index >= file.getFileText().size() || index < 0) {
    throw std::runtime_error("No symbol to erase: TextSize:" +
                             std::to_string(file.getFileText().size()));
  }

  Symbol s = file.getFileText().at(index);
  file.getFileText().remove(index);

  return s;
}

int Model::remoteInsert(Symbol &symbol) {
  return CrdtAlgorithm::remoteInsert(symbol, file.getFileText());
}

int Model::remoteErase(Symbol &symbol) {
  return CrdtAlgorithm::remoteErase(symbol, file.getFileText());
}

Symbol Model::generateSymbol(int index, QChar value) {
  auto pos1 = (index - 1 < file.getFileText().size() && index - 1 >= 0 &&
               !file.getFileText()[index - 1].getPos().empty())
                  ? file.getFileText()[index - 1].getPos()
                  : QVector<Identifier>();
  auto pos2 = (index < file.getFileText().size() && index >= 0 &&
               !file.getFileText()[index].getPos().empty())
                  ? file.getFileText()[index].getPos()
                  : QVector<Identifier>();
  auto newPos = CrdtAlgorithm::generatePosBetween(pos1, pos2, editorId);

  return std::move(Symbol(value, editorId, newPos, user.getUsername()));
}

void Model::setEditorId(unsigned int newEditorId) { editorId = newEditorId; }

unsigned Model::getEditorId() { return editorId; }

void Model::setCurrentFile(File &fileToSet) { file = fileToSet; }

void Model::setCurrentUser(User &userToSet) { user = std::move(userToSet); }

File &Model::getFile() { return file; }
