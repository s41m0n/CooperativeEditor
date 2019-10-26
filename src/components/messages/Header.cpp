#include "Header.h"

Header::Header(quint32 size, Type type) : size(size), type(type) {}

Header::Header() : size(0), type(Type::UNKNOWN){}

bool Header::isValid() {
  return size != 0;
}

std::string Header::toStdString(int level) {
  return std::string(level, '\t') + "Header{\n" +
         std::string(level + 1, '\t') + "msgSize: " +
         std::to_string(size) + "\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<std::underlying_type_t<Type>>(type)) + "\n" +
         std::string(level, '\t') + "}";
}

QDataStream &Header::serialize(QDataStream &stream) const {
  stream << size << type;
  return stream;
}

QDataStream &Header::deserialize(QDataStream &stream) {
  stream >> size >> type;
  return stream;
}

Type &Header::getType() {
  return type;
}

quint32 Header::getSize() {
  return size;
}

quint32 Header::objectSize() {
  return sizeof(type) + sizeof(size);
}
