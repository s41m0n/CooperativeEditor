#ifndef COOPERATIVEEDITOR_SYMBOL_H
#define COOPERATIVEEDITOR_SYMBOL_H

#include <vector>
#include <QDataStream>

#include "components/Identifier.h"

/**
 * Symbol class, to identify each inserted character
 *
 */
class Symbol {

private:
    ///The character represented
    char character;

    ///The site identifier
    unsigned int siteId;

    ///The position of this symbol
    std::vector<Identifier> position;

public:
    ///Classic constructor with all parameters
    Symbol(char character, unsigned int siteId,
           std::vector<Identifier> &position);

    ///Constructor to create a non-valid symbol to be filled
    Symbol();

    ///Method to compare two symbols
    int compareTo(const Symbol &s);

    ///Method to get the character
    char getChar();

    ///Method to get the position
    std::vector<Identifier> &getPos();

    ///Method to print in human-readable format the symbol
    std::string toString(int level = 0);

    ///Overload of the operators '<<' for QDataStream using Symbol for serialization
    friend QDataStream &operator<<(QDataStream &stream, const Symbol &val);

    ///Overload of the operators '>>' for QDataStream using Symbol for serialization
    friend QDataStream &operator>>(QDataStream &stream, Symbol &val);

};

///Overload of the operators '<<' for QDataStream using vector of Symbol for serialization
QDataStream &
operator<<(QDataStream &stream, std::vector<Symbol> &val);

///Overload of the operators '>>' for QDataStream using vector of Symbol for serialization
QDataStream &
operator>>(QDataStream &stream, std::vector<Symbol> &val);

#endif //COOPERATIVEEDITOR_SYMBOL_H
