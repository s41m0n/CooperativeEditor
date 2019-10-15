#include "Symbol.h"

Symbol::Symbol(QChar character, unsigned siteId,
               QVector<Identifier> &position) : position(std::move(position)),
                                                siteId(siteId),
                                                character(character) {
}

Symbol::Symbol() : character('\0'), position(), siteId(-1) {
}

QChar &Symbol::getChar() {
  return this->character;
}

QVector<Identifier> &Symbol::getPos() {
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

std::string Symbol::toStdString(int level) {
  std::string tmp(std::string(level, '\t') + "Symbol{\n" +
                  std::string(level + 1, '\t') + "SiteID: " +
                  std::to_string(siteId) + "\n" +
                  std::string(level + 1, '\t') + "character: " +
                  character.toLatin1() + "\n" +
                  std::string(level + 1, '\t') + "position: [");
  for (auto val: position)
    tmp += std::to_string(val.getDigit()) + ", ";
  tmp.erase(tmp.end() - 2, tmp.end());
  tmp += "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

QDataStream &operator<<(QDataStream &stream, const Symbol &val) {
  stream << val.character << val.siteId << val.position;
  return stream;
}

QDataStream &operator>>(QDataStream &stream, Symbol &val) {
  stream >> val.character >> val.siteId >> val.position;
  return stream;
}

QDataStream &operator>>(QDataStream &stream, QVector<Symbol> &val) {
  stream >> val;
  return stream;
}

QDataStream &operator<<(QDataStream &stream, QVector<Symbol> &val) {
  stream << val;
  return stream;
}