//
// Created by s41m0n on 16/05/19.
//

#include "components/Symbol.h"

Symbol::Symbol(char character, unsigned int siteId,
        std::vector<Identifier> &position) : position(std::move(position)),
        siteId(siteId), character(character) {
}

Symbol::Symbol() : character('\0'), position() {
}

char Symbol::getChar() {
  return this->character;
}

std::vector<Identifier> &Symbol::getPos() {
  return this->position;
}

int Symbol::compareTo(const Symbol &other) {
  auto pos1 = this->position;
  auto pos2 = other.position;

  for (int i = 0; i < std::min(pos1.size(), pos2.size()); i++) {
    auto digit1 = pos1[i];
    auto digit2 = pos2[i];
    auto comp = digit1.compareTo(digit2);

    if (comp != 0) {
      return comp;
    }
  }

  if (pos1.size() < pos2.size()) {
    return -1;
  } else if (pos1.size() > pos2.size()) {
    return 1;
  } else {
    return 0;
  }
}

std::string Symbol::toString(int level) {
  std::string tmp(std::string(level, '\t') + "Symbol{\n" +
                  std::string(level + 1, '\t') + "SiteID: " +
                  std::to_string(siteId) + "\n" +
                  std::string(level + 1, '\t') + "character: " + character +
                  "\n" +
                  std::string(level + 1, '\t') + "position: [");
  for (auto val: position)
    tmp += std::to_string(val.getDigit()) + ", ";
  tmp.erase(tmp.end() - 2, tmp.end());
  tmp += "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

QDataStream &operator<<(QDataStream &stream, const Symbol &val) {
  stream << val.character << val.siteId;
  stream << static_cast<quint32>(val.position.size());
  for (auto &tmp: val.position)
    stream << tmp;
  return stream;
}

QDataStream &operator>>(QDataStream &stream, Symbol &val) {
  quint32 size;
  stream >> reinterpret_cast<qint32 &>(val.character) >> val.siteId >> size;
  Identifier tmp;
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