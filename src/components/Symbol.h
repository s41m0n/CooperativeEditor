#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <QVector>
#include "src/include/lib/serialization/Serializable.h"

#include "Identifier.h"

enum Attribute {
    BOLD,
    ITALIC,
    UNDERLINED,
    ATTRIBUTE_SIZE
};

/**
 * Symbol class, to identify each inserted character
 *
 */
class Symbol : public Serializable {

private:

    QChar character;

    unsigned siteId;

    QVector<Identifier> position;

    bool attributes[Attribute::ATTRIBUTE_SIZE];

public:

    Symbol(QChar character, unsigned siteId,
           QVector<Identifier> &position);

    Symbol();

    int compareTo(const Symbol &s);

    QChar &getChar();

    QVector<Identifier> &getPos();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

    void setAttributes(const bool attributes[Attribute::ATTRIBUTE_SIZE]);

    bool isAttributeSet(Attribute attribute);
};

typedef QVector<Symbol> FileText;

#endif //COOPERATIVEEDITOR_SYMBOL_H
