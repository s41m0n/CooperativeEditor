#include "components/Identifier.h"

Identifier::Identifier(unsigned int editorId, unsigned int digit) : editorId(
        editorId), digit(digit) {}

int Identifier::compareTo(Identifier other) {
  if (this->digit < other.digit) {
    return -1;
  } else if (this->digit > other.digit) {
    return 1;
  } else {
    if (this->editorId < other.editorId) {
      return -1;
    } else if (this->editorId > other.editorId) {
      return 1;
    } else {
      return 0;
    }
  }
}

std::string Identifier::toString(int level) {
  return std::string(std::string(level, '\t') + "Identifier{\n" +
                     std::string(level + 1, '\t') + "editorId: " +
                     std::to_string(editorId) + "\n" +
                     std::string(level + 1, '\t') + "digit: " +
                     std::to_string(digit) + "\n" +
                     std::string(level, '\t') + "}");
}

QDataStream &operator<<(QDataStream &stream, const Identifier &val) {
  stream << val.editorId << val.digit;
  return stream;
}

QDataStream &operator>>(QDataStream &stream, Identifier &val) {
  stream >> val.editorId >> val.digit;
  return stream;
}

unsigned int Identifier::getEditorId() {
  return this->editorId;
}

unsigned int Identifier::getDigit() {
  return this->digit;
}