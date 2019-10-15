#ifndef COOPERATIVEEDITOR_IDENTIFIER_H
#define COOPERATIVEEDITOR_IDENTIFIER_H

#include <QDataStream>
#include <string>

/**
 * Identifier class, used to identify a symbol
 *
 */
class Identifier {

private:
    unsigned editorId;

    unsigned digit;

public:

    Identifier(unsigned int editorId, unsigned int digit);

    Identifier();

    int compareTo(Identifier other);

    friend QDataStream &operator<<(QDataStream &stream, const Identifier &val);

    friend QDataStream &operator>>(QDataStream &stream, Identifier &val);

    std::string toStdString(int level = 0);

    unsigned getEditorId();

    unsigned getDigit();

};


#endif //COOPERATIVEEDITOR_IDENTIFIER_H
