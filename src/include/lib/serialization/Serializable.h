#ifndef COOPERATIVEEDITOR_SERIALIZABLE_H
#define COOPERATIVEEDITOR_SERIALIZABLE_H

#include "src/components/messages/MessageType.h"
#include <QDataStream>
#include <string>

/**
 * Serializable class which defines how object should be serialized
 */
class Serializable {

public:
  /**
   * Function to be called to deserialize
   * @param stream the stream containing the data
   * @return the stream
   */
  virtual QDataStream &deserialize(QDataStream &stream) = 0;

  /**
   * Function to be called when serialized data
   * @param stream  the stream where to serialize
   * @return  the stream
   */
  virtual QDataStream &serialize(QDataStream &stream) const = 0;

  /**
   * Function to print this object as a std::string
   * @param level the indentation level
   * @return  the string version of the object
   */
  virtual std::string toStdString(int level = 0) = 0;
};

/*Defining once for all the operators used with the data stream*/

QDataStream &operator<<(QDataStream &stream, const Serializable &val);

QDataStream &operator>>(QDataStream &stream, Serializable &val);

QDataStream &operator<<(QDataStream &stream, const Type &val);

QDataStream &operator>>(QDataStream &stream, Type &val);

#endif // COOPERATIVEEDITOR_SERIALIZABLE_H
