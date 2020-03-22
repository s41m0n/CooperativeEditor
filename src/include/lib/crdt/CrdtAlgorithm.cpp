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

int CrdtAlgorithm::findPositionErase(Symbol &s, FileText &symbols) {
  int left = 0, mid;
  auto right = symbols.size() - 1;

  // check if struct is empty or char is less than first char
  if (symbols.empty() || s.compareTo(symbols[left]) < 0)
    return 0;
  else if (s.compareTo(symbols[right]) > 0)
    return symbols.size();

  // binary search
  while (left <= right) {
    mid = std::floor((right + left) / 2.0);

    int comp = s.compareTo(symbols[mid]);
    if (comp == 0)
      return mid;
    else if (comp < 0)
      right = mid - 1;
    else
      left = mid + 1;
  }
  return -1;
}

int CrdtAlgorithm::findPositionInsert(Symbol &s, FileText &symbols) {
  int left = 0;
  auto right = symbols.size() - 1;
  int mid, compareNum;

  if (symbols.empty() || s.compareTo(symbols[left]) < 0)
    return left;
  else if (s.compareTo(symbols[right]) > 0)
    return symbols.size();

  while (left + 1 < right) {
    mid = std::floor(left + (right - left) / 2);
    compareNum = s.compareTo(symbols[mid]);

    if (compareNum == 0)
      return mid;
    else if (compareNum > 0)
      left = mid;
    else
      right = mid;
  }

  if (s.compareTo(symbols[left]) == 0)
    return left;
  else
    return right;
}

QVector<Identifier> CrdtAlgorithm::generatePosBetween(
        QVector<Identifier> pos1, QVector<Identifier> pos2,
        unsigned int editorId, QVector<Identifier> newPos, int level) {

  auto baseValue = (unsigned int) (std::pow(2, level) * CrdtAlgorithm::base);
  if (!baseValue) {
    baseValue = UINT_MAX;
  }
  bool boundaryStrategy = retrieveStrategy(level);

  auto id1 = !pos1.empty() ? pos1[0] : Identifier(editorId, 0);
  auto id2 = !pos2.empty() ? pos2[0] : Identifier(editorId, baseValue);

  if (id2.getDigit() - id1.getDigit() > 1) {
    auto newDigit = CrdtAlgorithm::generateIdBetween(id1.getDigit(),
                                                     id2.getDigit(),
                                                     boundaryStrategy);
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

int CrdtAlgorithm::remoteErase(Symbol &s, FileText &symbols) {
  int index = CrdtAlgorithm::findPositionErase(s, symbols);

  if (index < 0) {
    throw std::runtime_error("Remote erase bad index" + std::to_string(index));
  }

  symbols.erase(symbols.begin() + index);
  return index;
}

int CrdtAlgorithm::remoteInsert(Symbol &s, FileText &symbols) {
  int index = CrdtAlgorithm::findPositionInsert(s, symbols);

  if (index < 0) {
    throw std::runtime_error("Remote insert bad index" + std::to_string(index));
  }

  symbols.insert(symbols.begin() + index, s);
  return index;
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
    }
      break;
    default:
      strategySelected = ((level % 2) == 0);
      break;
  }

  return strategySelected;
}

int CrdtAlgorithm::replaceSymbol(Symbol &s, FileText &symbols) {
  int index = CrdtAlgorithm::findPositionErase(s, symbols);

  if (index < 0) {
    throw std::runtime_error("Remote update bad index" + std::to_string(index));
  }

  symbols.erase(symbols.begin() + index);
  symbols.insert(symbols.begin() + index, s);
  return index;
}
