#include "Model.h"

Model::Model() : editorId(0), digitGenerator(0), file(), user() {
}

User Model::getUser() {
  return this->user;
}

FileText &Model::getFileText() {
  return this->file.getFileText();
}

std::string Model::toStdString() {
  std::string str;
  for (auto &val : file.getFileText()) {
    str += val.getChar().toLatin1();
  }
  return str;
}

QVector<Symbol>
Model::localInsert(int index, QString value, const QVector<bool> &attributes) {

  if (index > file.getFileText().size() || index < 0) {
    throw std::runtime_error("No valid position: TextSize:" +
                             std::to_string(file.getFileText().size()));
  }

  QVector<Symbol> toReturn;
  for (int i = 0; i < value.size(); i++) {
    Symbol s = generateSymbol(index + i, value[i]);
    s.setAttributes(attributes);
    file.getFileText().insert(file.getFileText().begin() + index + i, s);
    toReturn.push_back(s);
  }

  return toReturn;
}

QVector<Symbol> Model::localErase(int index, int size) {

  if (index >= file.getFileText().size() || index < 0) {
    throw std::runtime_error("No symbol to erase: TextSize:" +
                             std::to_string(file.getFileText().size()));
  }

  QVector<Symbol> toReturn;
  toReturn.append(file.getFileText().mid(index, size));
  file.getFileText().remove(index, size);

  return toReturn;
}

QVector<Symbol>
Model::localUpdate(int index, int size, Attribute attribute, bool set) {
  if (index >= file.getFileText().size() || index < 0) {
    throw std::runtime_error("No symbol to update: TextSize:" +
                             std::to_string(file.getFileText().size()));
  }

  QVector<Symbol> toReturn;
  for (int i = 0; i < size; ++i) {
    Symbol &s = file.getFileText()[index + i];
    s.setAttribute(attribute, set);
    toReturn.push_back(s);
  }

  return toReturn;
}

int Model::remoteInsert(QVector<Symbol> symbol) {
  return CrdtAlgorithm::remoteInsert(symbol, file.getFileText());
}

int Model::remoteErase(QVector<Symbol> symbol) {
  return CrdtAlgorithm::remoteErase(symbol, file.getFileText());
}

int Model::remoteUpdate(QVector<Symbol> symbol) {
  return CrdtAlgorithm::replaceSymbol(symbol, file.getFileText());
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

File Model::getFile() {
  return this->file;
}
