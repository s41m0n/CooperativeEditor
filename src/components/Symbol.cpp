//
// Created by s41m0n on 16/05/19.
//

#include "Symbol.h"

Symbol::Symbol(char character, unsigned int siteId, unsigned int digit,
               std::vector<int> &position)
        : position(std::move(position)), id(siteId, digit),
          character(character) {
}

Symbol::Symbol() : id(), character('\0'), position() {
}

char Symbol::getChar() {
  return this->character;
}

std::vector<int> &Symbol::getPos() {
  return this->position;
}

int Symbol::compareTo(const Symbol &other) {
  int digit1, digit2;
  auto pos1 = this->position;
  auto pos2 = other.position;

  for (int i = 0; i < std::min(pos1.size(), pos2.size()); i++) {
    digit1 = pos1[i];
    digit2 = pos2[i];
    if (digit1 != digit2)
      return digit1 > digit2 ? 1 : -1;
  }

  if (pos1.size() < pos2.size()) {
    return -1;
  } else if (pos1.size() > pos2.size()) {
    return 1;
  } else {
    return this->id.compareTo(other.id);
  }
}

std::string Symbol::toString(int level) {
  std::string tmp(std::string(level, '\t') + "Symbol{\n" +
                  id.toString(level + 1) + "\n" +
                  std::string(level + 1, '\t') + "character: " + character +
                  "\n" +
                  std::string(level + 1, '\t') + "position: [");
  for (auto val: position)
    tmp += std::to_string(val) + ", ";
  tmp.erase(tmp.end() - 2, tmp.end());
  tmp += "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

QDataStream &operator<<(QDataStream &stream, const Symbol &val) {
  stream << val.character << val.id;
  stream << static_cast<quint32>(val.position.size());
  for (auto &tmp: val.position)
    stream << tmp;
  return stream;
}

QDataStream &operator>>(QDataStream &stream, Symbol &val) {
  quint32 size;
  stream >> reinterpret_cast<qint32 &>(val.character) >> val.id >> size;
  int tmp;
  for (quint32 i = 0; i < size; i++) {
    stream >> tmp;
    val.position.emplace_back(tmp);
  }
  return stream;
}

QDataStream &operator>>(QDataStream &stream, std::vector<Symbol> &val) {
  quint32 length = 0;
  stream >> length;
  for (quint32 i = 0; i < length; ++i) {
    Symbol obj;
    stream >> obj;
    val.push_back(obj);
  }
  return stream;
}

QDataStream &operator<<(QDataStream &stream, const std::vector<Symbol> &val) {
  stream << static_cast<quint32>(val.size());
  for (const auto &v : val) {
    stream << v;
  }
  return stream;
}