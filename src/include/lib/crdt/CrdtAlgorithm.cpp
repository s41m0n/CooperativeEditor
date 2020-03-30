#include <cmath>
#include <random>

#include "CrdtAlgorithm.h"

unsigned int CrdtAlgorithm::generateIdBetween(unsigned int id1,
                                              unsigned int id2,
                                              bool boundaryStrategy) {
  if ((id2 - id1) < CrdtAlgorithm::boundary) {
    id1++;
    id2--;
  } else {
    if (boundaryStrategy) {
      id1++;
      id2 = id1 + CrdtAlgorithm::boundary;
    } else {
      id2--;
      id1 = id2 - CrdtAlgorithm::boundary;
    }
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(id1, id2);
  return distribution(gen);
}

QVector<Identifier> CrdtAlgorithm::generatePosBetween(
    QVector<Identifier> pos1, QVector<Identifier> pos2, unsigned int editorId,
    QVector<Identifier> newPos, int level) {

  auto baseValue = (unsigned int)(std::pow(2, level) * CrdtAlgorithm::base);
  if (!baseValue) {
    baseValue = UINT_MAX;
  }
  bool boundaryStrategy = retrieveStrategy(level);

  auto id1 = !pos1.empty() ? pos1[0] : Identifier(editorId, 0);
  auto id2 = !pos2.empty() ? pos2[0] : Identifier(editorId, baseValue);

  if (id2.getDigit() - id1.getDigit() > 1) {
    auto newDigit = CrdtAlgorithm::generateIdBetween(
        id1.getDigit(), id2.getDigit(), boundaryStrategy);
    newPos.push_back({editorId, newDigit});
    return newPos;
  } else if (id2.getDigit() - id1.getDigit() == 1) {
    newPos.push_back(id1);
    if (!pos1.empty()) {
      pos1.erase(pos1.begin());
    }
    return CrdtAlgorithm::generatePosBetween(pos1, {}, editorId, newPos,
                                             level + 1);
  } else if (id1.getDigit() == id2.getDigit()) {
    if (id1.getEditorId() < id2.getEditorId()) {
      newPos.push_back(id1);
      if (!pos1.empty()) {
        pos1.erase(pos1.begin());
      }
      return CrdtAlgorithm::generatePosBetween(pos1, {}, editorId, newPos,
                                               level + 1);
    } else if (id1.getEditorId() == id2.getEditorId()) {
      newPos.push_back(id1);
      if (!pos1.empty()) {
        pos1.erase(pos1.begin());
      }
      if (!pos2.empty()) {
        pos2.erase(pos2.begin());
      }
      return CrdtAlgorithm::generatePosBetween(pos1, pos2, editorId, newPos,
                                               level + 1);
    } else {
      throw std::runtime_error("Fix Position Sorting");
    }
  }
  throw std::runtime_error("Fix Position Sorting");
}

void CrdtAlgorithm::remoteErase(Symbol &s, FileText &symbols) {
  auto pos = findPosition(s, symbols);

  if (pos.first == -1 && pos.second == -1) return;

  symbols[pos.first].remove(pos.second);

  if (s.getChar() == "\n" && pos.first + 1 < symbols.size()) {
    mergeLines(pos.first, symbols);
  }

  removeEmptyLines(symbols);
}

void CrdtAlgorithm::removeEmptyLines(FileText &symbols) {
  for (int line = 0; line < symbols.size(); line++) {
    if (symbols[line].empty()) {
      symbols.remove(line);
      line--;
    }
  }

  if (symbols.empty()) {
    symbols.append(QVector<Symbol>());
  }
}

// when deleting newline, concat line with next line
void CrdtAlgorithm::mergeLines(int fromLine, FileText &symbols) {
  symbols[fromLine].append(symbols[fromLine + 1]);
  symbols.remove(fromLine + 1);
}

std::pair<int, int> CrdtAlgorithm::findPositionInsert(Symbol &s,
                                                      FileText &symbols) {
  int minLine = 0;
  int totalLines = symbols.size();
  int maxLine = totalLines - 1;
  auto lastLine = symbols[maxLine];
  int midLine;
  QVector<Symbol> *currentLine;

  if (symbols.isEmpty() || s.compareTo(symbols[0][0]) < 0) {
    return std::make_pair(0, 0);
  }

  Symbol *lastChar = &lastLine[lastLine.size() - 1];

  if (s.compareTo(*lastChar) > 0) {
    return findEndPosition(*lastChar, lastLine, totalLines);
  }

  while (minLine + 1 < maxLine) {
    midLine = std::floor(minLine + (maxLine - minLine) / 2);
    currentLine = &symbols[midLine];
    lastChar = const_cast<Symbol *>(&currentLine->at(currentLine->size() - 1));

    if (s.compareTo(*lastChar) == 0) {
      return std::make_pair(midLine, currentLine->size() - 1);
    } else if (s.compareTo(*lastChar) < 0) {
      maxLine = midLine;
    } else {
      minLine = midLine;
    }
  }

  auto minCurrentLine = symbols[minLine];
  auto minLastChar = minCurrentLine[minCurrentLine.size() - 1];
  auto maxCurrentLine = symbols[maxLine];
  auto maxLastChar = maxCurrentLine[maxCurrentLine.size() - 1];

  if (s.compareTo(minLastChar) <= 0) {
    int index = findInsertIndexInLine(s, minCurrentLine, symbols);
    return std::make_pair(minLine, index);
  } else {
    int index = findInsertIndexInLine(s, maxCurrentLine, symbols);
    return std::make_pair(maxLine, index);
  }
}

std::pair<int, int> CrdtAlgorithm::findEndPosition(Symbol &lastChar,
                                                   QVector<Symbol> &lastLine,
                                                   int totalLines) {
  if (lastChar.getChar() == "\n") {
    return std::make_pair(totalLines, 0);
  } else {
    return std::make_pair(totalLines - 1, lastLine.size());
  }
}

// binary search to find char in a line
int CrdtAlgorithm::findInsertIndexInLine(Symbol &s, QVector<Symbol> &line,
                                         FileText &symbols) {
  int left = 0;
  int right = line.size() - 1;
  int mid, compareNum;

  if (line.empty() || s.compareTo(line[left]) < 0) {
    return left;
  } else if (s.compareTo(line[right]) > 0) {
    return symbols.size();
  }

  while (left + 1 < right) {
    mid = std::floor(left + (right - left) / 2);
    compareNum = s.compareTo(line[mid]);

    if (compareNum == 0) {
      return mid;
    } else if (compareNum > 0) {
      left = mid;
    } else {
      right = mid;
    }
  }

  if (s.compareTo(line[left]) == 0) {
    return left;
  } else {
    return right;
  }
}

int CrdtAlgorithm::findIndexInLine(Symbol &s, QVector<Symbol> &line,
                                   FileText &symbols) {
  int left = 0;
  int right = line.size() - 1;
  int mid, compareNum;

  if (line.empty() || s.compareTo(line[left]) < 0) {
    return left;
  } else if (s.compareTo(line[right]) > 0) {
    return symbols.size();
  }

  while (left + 1 < right) {
    mid = std::floor(left + (right - left) / 2);
    compareNum = s.compareTo(line[mid]);

    if (compareNum == 0) {
      return mid;
    } else if (compareNum > 0) {
      left = mid;
    } else {
      right = mid;
    }
  }

  if (s.compareTo(line[left]) == 0) {
    return left;
  } else if (s.compareTo(line[right]) == 0) {
    return right;
  } else {
    return false;
  }
}

void CrdtAlgorithm::insertSymbol(Symbol &symbol, int line, int index,
                                 FileText &symbols) {
  if (line == symbols.size()) {
    symbols.append(QVector<Symbol>());
  }

  // if inserting a newline, split line into two lines
  if (symbol.getChar() == "\n") {
    auto lineAfter = symbols[line].mid(index);
    symbols[line].remove(index, symbols[line].size()-index);

    if (lineAfter.empty()) {
      symbols[line].insert(index, symbol);
    } else {
      auto lineBefore = symbols[line];
      lineBefore.append(symbol);
      symbols.remove(line);
      symbols.insert(line, lineAfter);
      symbols.insert(line, lineBefore);
    }
  } else {
    symbols[line].insert(index, symbol);
  }
}

void CrdtAlgorithm::remoteInsert(Symbol &s, FileText &symbols) {
  auto index = CrdtAlgorithm::findPositionInsert(s, symbols);

  insertSymbol(s, index.first, index.second, symbols);
}

bool CrdtAlgorithm::retrieveStrategy(int level) {

  bool strategySelected;
  switch (CrdtAlgorithm::strategy) {
  case Strategy::PLUS:
    strategySelected = true;
    break;
  case Strategy::MINUS:
    strategySelected = false;
    break;
  case Strategy::RANDOM: {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, 1);
    strategySelected = distribution(gen) != 0;
  } break;
  default:
    strategySelected = ((level % 2) == 0);
    break;
  }

  return strategySelected;
}
/*
int CrdtAlgorithm::replaceSymbol(Symbol &s, FileText &symbols) {
  int index = CrdtAlgorithm::findPositionErase(s, symbols);

  if (index < 0) {
    throw std::runtime_error("Remote update bad index" + std::to_string(index));
  }

  symbols.erase(symbols.begin() + index);
  symbols.insert(symbols.begin() + index, s);
  return index;
}
*/

std::pair<int,int> CrdtAlgorithm::findPosition(Symbol &s, FileText &symbols) {
  int minLine = 0;
  int totalLines = symbols.size();
  int maxLine = totalLines - 1;
  auto lastLine = symbols[maxLine];
  int midLine;
  QVector<Symbol> *currentLine;

  // check if struct is empty or char is less than first char
  if (symbols.isEmpty() || s.compareTo(symbols[0][0]) < 0) {
    return std::make_pair(-1, -1);
  }

  Symbol *lastChar = &lastLine[lastLine.size() - 1];

  // char is greater than all existing chars (insert at end)
  if (s.compareTo(*lastChar) > 0) {
    return std::make_pair(-1, -1);
  }

  // binary search
  while (minLine + 1 < maxLine) {
    midLine = std::floor(minLine + (maxLine - minLine) / 2);
    currentLine = &symbols[midLine];
    lastChar = const_cast<Symbol *>(&currentLine->at(currentLine->size() - 1));

    if (s.compareTo(*lastChar) == 0) {
      return std::make_pair(midLine, currentLine->size()-1);
    } else if (s.compareTo(*lastChar) < 0) {
      maxLine = midLine;
    } else {
      minLine = midLine;
    }
  }

  // Check between min and max line.
  auto minCurrentLine = symbols[minLine];
  auto minLastChar = minCurrentLine[minCurrentLine.size() - 1];
  auto maxCurrentLine = symbols[maxLine];
  auto maxLastChar = maxCurrentLine[maxCurrentLine.size() - 1];

  if (s.compareTo(minLastChar) <= 0) {
    int index = findIndexInLine(s, minCurrentLine, symbols);
    return std::make_pair(minLine, index);
  } else {
    int index = findIndexInLine(s, maxCurrentLine, symbols);
    return std::make_pair(maxLine, index);
  }
}