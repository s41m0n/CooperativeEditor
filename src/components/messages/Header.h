#ifndef COOPERATIVEEDITOR_HEADER_H
#define COOPERATIVEEDITOR_HEADER_H

#include "src/include/lib/serialization/Serializable.h"

class Header : public Serializable {

private:
  quint32 size;

  Type type;

public:
  Header(quint32 size, Type type);

  Header();

  bool isValid();

  Type &getType();

  quint32 getSize();

  std::string toStdString(int level = 0) override;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

  static const quint32 OBJECT_SIZE =
      sizeof(type) + sizeof(std::underlying_type_t<Type>);
};

#endif // COOPERATIVEEDITOR_HEADER_H
