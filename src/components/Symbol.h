#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <QVector>
#include <QDataStream>

#include "Identifier.h"

/**
 * Symbol class, to identify each inserted character
 *
 */
class Symbol {

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

    std::string toStdString(int level = 0);

    friend QDataStream &operator<<(QDataStream &stream, const Symbol &val);

    friend QDataStream &operator>>(QDataStream &stream, Symbol &val);

};

typedef QVector<Symbol> FileText;

#endif //COOPERATIVEEDITOR_SYMBOL_H
