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

    quint32 objectSize();
};


#endif //COOPERATIVEEDITOR_HEADER_H
