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
  tmp += "]\n";
  tmp += std::string(level + 1, '\t') + "attributes: [";
  for(auto attr : attributes){
    if(attr){
      tmp += "TRUE, ";
    }else{
      tmp += "FALSE, ";
    }
  }
  tmp.erase(tmp.end() - 2, tmp.end());
  tmp += "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

QDataStream &Symbol::serialize(QDataStream &stream) const {
  stream << character << siteId << position;
  for (int i = 0; i < Attribute::ATTRIBUTE_SIZE; i++) { // NOLINT(modernize-loop-convert)
    stream << attributes[i];
  }
  return stream;
}

QDataStream &Symbol::deserialize(QDataStream &stream) {
  stream >> character >> siteId >> position;

  for (int i = 0; i < Attribute::ATTRIBUTE_SIZE; i++) { // NOLINT(modernize-loop-convert)
    stream >> attributes[i];
  }
  return stream;
}

void Symbol::setAttributes(const bool attr[Attribute::ATTRIBUTE_SIZE]) {
  for (int i = 0; i < Attribute::ATTRIBUTE_SIZE; i++) {
    attributes[i] = attr[i];
  }
}

bool Symbol::isAttributeSet(Attribute attribute) {
  return attributes[attribute];
}
