#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <QVector>
#include "common/Serializable.h"

#include "Identifier.h"

/**
 * Symbol class, to identify each inserted character
 *
 */
class Symbol : public Serializable {

private:

    QChar character;

    unsigned siteId;

    QVector<Identifier> position;

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

};

typedef QVector<Symbol> FileText;

#endif //COOPERATIVEEDITOR_SYMBOL_H
