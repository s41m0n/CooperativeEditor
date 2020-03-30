#include "Model.h"

User Model::getUser() { return this->user; }

FileText Model::getFileText() { return file; }

QString Model::textify() {
  QString str;
  for (auto &val : file) {
    for (auto &s : val) {
      str += s.getChar();
    }
    str += "|";
  }
  return str;
}

QVector<Identifier> Model::findPosBefore(int line, int index) {

  if (index == 0) {
    if (line == 0)
      return QVector<Identifier>();
    line = line - 1;
    index = file[line].size();
  }

  return file[line][index - 1].getPos();
}

QVector<Identifier> Model::findPosAfter(int line, int index) {

  int numLines = file.size();
  int numChars = line < numLines ? file[line].size() : 0;

  if ((line == numLines - 1) && (index == numChars)) {
    return {};
  } else if ((line < numLines - 1) && (index == numChars)) {
    line = line + 1;
    index = 0;
  } else if ((line > numLines - 1) && index == 0) {
    return {};
  }

  return file[line][index].getPos();
}

Symbol Model::generateSymbol(int line, int index, QChar value) {
  auto pos1 = findPosBefore(line, index);
  auto pos2 = findPosAfter(line, index);
  auto newPos = CrdtAlgorithm::generatePosBetween(pos1, pos2, editorId);

  return std::move(Symbol(value, editorId, newPos));
}



Symbol Model::localInsert(int line, int index, QChar value,
                                   const QVector<bool> &attributes) {

  Symbol s = generateSymbol(line, index, value);
  s.setAttributes(attributes);
  CrdtAlgorithm::insertSymbol(s, line, index, file);

  return s;
}

QVector<Symbol> Model::localErase(int fromLine, int fromIndex, int toLine,
                                  int toIndex) {
  QVector<Symbol> chars;
  bool newlineRemoved = false;

  // for multi-line deletes
  if (fromLine != toLine) {
    // delete chars on first line from startPos.ch to end of line
    newlineRemoved = true;
    chars = deleteMultipleLines(fromLine, fromIndex, toLine, toIndex);

    // single-line deletes
  } else {
    chars = deleteSingleLine(fromLine, fromIndex, toLine, toIndex);

    auto it = std::find_if(chars.begin(), chars.end(), [](Symbol &val) {
      return val.getChar() == "\n";
    });
    if (it != chars.end())
      newlineRemoved = true;
  }

  CrdtAlgorithm::removeEmptyLines(file);

  if (newlineRemoved && fromLine + 1 < file.size()) {
    CrdtAlgorithm::mergeLines(fromLine, file);
  }
  return chars;
}


QVector<Symbol> Model::deleteSingleLine(int fromLine, int fromIndex, int toLine,
                                        int toIndex) {
  auto charNum = toIndex - fromIndex;
  auto chars = file[fromLine].mid(fromIndex, charNum);
  file[fromLine].remove(fromIndex, charNum);

  return chars;
}

QVector<Symbol> Model::deleteMultipleLines(int fromLine, int fromIndex,
                                           int toLine, int toIndex) {
  QVector<Symbol> chars = file[fromLine].mid(fromIndex);
  file[fromLine].remove(fromIndex, file[fromLine].size());

  for (int line = fromLine + 1; line < toLine; line++) {
    chars.append(file[line]);
    file.remove(line);
  }

  if (!file[toLine].empty()) {
    chars.append(file[toLine].mid(0, toIndex));
    file[toLine].remove(0, toIndex);
  }

  return chars;
}

void Model::remoteInsert(Symbol &symbol) {
    CrdtAlgorithm::remoteInsert(symbol, file);
}

void Model::remoteErase(Symbol &symbol) {
  CrdtAlgorithm::remoteErase(symbol, file);
}

void Model::setCurrentFile(File &fileToSet) {
  file = fileToSet.getFileText();
}
void Model::setEditorId(unsigned newEditorId) {
  editorId = newEditorId;
}
