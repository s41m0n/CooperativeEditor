#ifndef COOPERATIVEEDITOR_IDENTIFIER_H
#define COOPERATIVEEDITOR_IDENTIFIER_H

#include <QtCore/QDataStream>
#include <string>

/**
 * Identifier class, used to identify a symbol
 *
 * @author Simone Magnani - s41m0n
 *
 */
class Identifier {

private:
    ///The editor specific ID
    unsigned int editorId;

    ///Unique digit inside the editor
    unsigned int digit;

public:
    ///Classic constructor, given both the two parameter
    Identifier(unsigned int editorId, unsigned int digit);

    ///Constructor used to create a non-valid Identifier to be filled
    Identifier() = default;

    ///Method to compare two identifier by their private variables
    int compareTo(Identifier other);

    ///Operator overload '<<' for Identifier when using QDataStream for serialization
    friend QDataStream &operator<<(QDataStream &stream, const Identifier &val);

    ///Operator overload '>>' for Identifier when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, Identifier &val);

    ///Method to print in human-readable format the Identifier
    std::string toString(int level = 0);

    ///Return editor identifier
    unsigned int getEditorId();

    ///Return digit
    unsigned int getDigit();

};


#endif //COOPERATIVEEDITOR_IDENTIFIER_H
