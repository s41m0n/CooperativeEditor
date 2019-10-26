#ifndef COOPERATIVEEDITOR_IDENTIFIER_H
#define COOPERATIVEEDITOR_IDENTIFIER_H

#include <string>
#include <common/Serializable.h>

/**
 * Identifier class, used to identify a symbol
 *
 */
class Identifier : public Serializable {

private:
    unsigned editorId;

    unsigned digit;

public:

    Identifier(unsigned int editorId, unsigned int digit);

    Identifier();

    int compareTo(const Identifier &other);

    unsigned getEditorId();

    unsigned getDigit();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

    quint32 objectSize() override;

};


#endif //COOPERATIVEEDITOR_IDENTIFIER_H
