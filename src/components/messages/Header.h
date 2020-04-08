#ifndef COOPERATIVEEDITOR_HEADER_H
#define COOPERATIVEEDITOR_HEADER_H

#include "src/include/lib/serialization/Serializable.h"

/**
 * Header class, representing the header of a message
 */
class Header : public Serializable {

private:
  quint32 size;

  Type type;

public:
  Header(quint32 size, Type type);

  Header();

  std::string toStdString(int level = 0) override;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

  /**
   * Function to check if this header is valid
   * @return  true if size != 0, false otherwise
   */
  bool isValid();

  /**
   * Function to get the type of the header
   * @return  the header type
   */
  Type &getType();

  /**
   * Function to get the size of the message following the header
   * @return the message size
   */
  quint32 getSize();

  /// Static definition of a header size
  static const quint32 OBJECT_SIZE =
      sizeof(size) + sizeof(std::underlying_type_t<Type>);
};

#endif // COOPERATIVEEDITOR_HEADER_H
