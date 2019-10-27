#include "Serializable.h"

QDataStream &operator<<(QDataStream &stream, const Type &val) {
  stream << static_cast<std::underlying_type_t<Type>>(val);
  return stream;
}

QDataStream &operator>>(QDataStream &stream, Type &val) {
  stream >> reinterpret_cast<std::underlying_type_t<Type>&>(val);
  return stream;
}

QDataStream &operator<<(QDataStream &stream, const Serializable &val) {
  return val.serialize(stream);
}

QDataStream &operator>>(QDataStream &stream, Serializable &val) {
  return val.deserialize(stream);
}