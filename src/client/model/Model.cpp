#include "Model.h"

User Model::getUser() { return this->user; }

FileText Model::getFileText() { return file.getFileText(); }

QString Model::getFileName() { return file.getFileName(); }

QString Model::textify() {
  QString str;
  for (auto &val : file.getFileText()) {
    for (auto &s : val) {
      str += s.getChar();
    }
  }
  return str;
}

QVector<Identifier> Model::findPosBefore(int line, int index) {

  auto &symbols = file.getFileText();
  if (index == 0) {
    if (line == 0)
      return {};
    line = line - 1;
    index = symbols[line].size();
  }

  return symbols[line][index - 1].getPos();
}

QVector<Identifier> Model::findPosAfter(int line, int index) {

  auto &symbols = file.getFileText();
  int numLines = symbols.size();
  int numChars =
      line < numLines && !symbols[line].empty() ? symbols[line].size() : 0;

  if ((line == numLines - 1) && (index == numChars)) {
    return {};
  } else if ((line < numLines - 1) && (index == numChars)) {
    line = line + 1;
    index = 0;
  } else if ((line > numLines - 1) && index == 0) {
    return {};
  }

  return symbols[line][index].getPos();
}

Symbol Model::generateSymbol(int line, int index, QChar value) {
  auto pos1 = findPosBefore(line, index);
  auto pos2 = findPosAfter(line, index);
  auto newPos = CrdtAlgorithm::generatePosBetween(pos1, pos2, editorId);

  return std::move(Symbol(value, editorId, newPos));
}

Symbol Model::localInsert(int index, QChar value,
                          const QVector<bool> &attributes) {

  auto pos = matricize(index);
  Symbol s = generateSymbol(pos.first, pos.second, value);
  s.setAttributes(attributes);
  CrdtAlgorithm::insertSymbol(s, pos.first, pos.second, file.getFileText());

  return s;
}

QVector<Symbol> Model::localErase(int from, int offset) {
  auto pos1 = matricize(from);
  auto pos2 = matricize(from + offset);

  auto &symbols = file.getFileText();
  QVector<Symbol> chars;
  bool newlineRemoved = false;

  // for multi-line deletes
  if (pos1.first != pos2.first) {
    // delete chars on first line from startPos.ch to end of line
    newlineRemoved = true;
    chars =
        deleteMultipleLines(pos1.first, pos1.second, pos2.first, pos2.second);

    // single-line deletes
  } else {
    chars = deleteSingleLine(pos1.first, pos1.second, pos2.first, pos2.second);

    auto it = std::find_if(chars.begin(), chars.end(),
                           [](Symbol &val) { return val.getChar() == '\n'; });
    if (it != chars.end())
      newlineRemoved = true;
  }

  CrdtAlgorithm::removeEmptyLines(symbols);

  if (newlineRemoved && pos1.first + 1 < symbols.size()) {
    CrdtAlgorithm::mergeLines(pos1.first, symbols);
  }
  return chars;
}

QVector<Symbol> Model::deleteSingleLine(int fromLine, int fromIndex, int toLine,
                                        int toIndex) {
  auto &symbols = file.getFileText();
  auto charNum = toIndex - fromIndex;
  auto chars = symbols[fromLine].mid(fromIndex, charNum);
  symbols[fromLine].remove(fromIndex, charNum);

  return chars;
}

QVector<Symbol> Model::deleteMultipleLines(int fromLine, int fromIndex,
                                           int toLine, int toIndex) {
  auto &symbols = file.getFileText();

  QVector<Symbol> chars = symbols[fromLine].mid(fromIndex);
  symbols[fromLine].erase(symbols[fromLine].begin() + fromIndex, symbols[fromLine].end());

  for (int line = fromLine + 1; line < toLine; line++) {
    chars.append(symbols[line]);
    symbols.remove(line);
  }

  if (!symbols[toLine].empty()) {
    chars.append(symbols[toLine].mid(0, toIndex));
    symbols[toLine].remove(0, toIndex);
  }

  return chars;
}

int Model::remoteInsert(Symbol &symbol) {
  return linearize(CrdtAlgorithm::remoteInsert(symbol, file.getFileText()));
}

int Model::remoteErase(Symbol &symbol) {
  return linearize(CrdtAlgorithm::remoteErase(symbol, file.getFileText()));
}

void Model::setCurrentFile(File &fileToSet) { file = fileToSet; }
void Model::setEditorId(unsigned newEditorId) { editorId = newEditorId; }
unsigned Model::getEditorId() { return editorId; }

void Model::setCurrentUser(User &userToSet) { user = userToSet; }

std::pair<int, int> Model::matricize(int index) {
  int row_counter = 0;

  auto &text = file.getFileText();

  for(auto &row : file.getFileText()) {
    if(index <= row.size()) {
      break;
    }
    row_counter++;
    index -= row.size();
  }

  return std::make_pair(row_counter, index);
}

int Model::linearize(std::pair<int, int> pos) {
  int index = 0;

  auto rowIterator = file.getFileText().begin();
  auto rowEnd = rowIterator + pos.first;

  while (rowIterator != rowEnd) {
    index += rowIterator->size();
    rowIterator++;
  }

  index += pos.second;

  return index;
}
