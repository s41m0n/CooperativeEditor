#ifndef COOPERATIVEEDITOR_SERIALIZABLE_H
#define COOPERATIVEEDITOR_SERIALIZABLE_H

#include <QDataStream>
#include <string>
#include "src/components/messages/MessageType.h"

class Serializable {

public:
  /// Method to be called when deserialized through a QDataStream
  virtual QDataStream &deserialize(QDataStream &stream) = 0;

  /// Method to be called when serialized through a QDataStream
  virtual QDataStream &serialize(QDataStream &stream) const = 0;

  /// Method to print in human-readable format the message using indent
  virtual std::string toStdString(int level = 0) = 0;
};

QDataStream &operator<<(QDataStream &stream, const Serializable &val);

QDataStream &operator>>(QDataStream &stream, Serializable &val);

QDataStream &operator<<(QDataStream &stream, const Type &val);

QDataStream &operator>>(QDataStream &stream, Type &val);

#endif // COOPERATIVEEDITOR_SERIALIZABLE_H
