#ifndef COOPERATIVEEDITOR_FILECONTENTMESSAGE_H
#define COOPERATIVEEDITOR_FILECONTENTMESSAGE_H

#include "BasicMessage.h"
#include "components/Symbol.h"

/**
 * FileContentMessage class, represents a File content transfer message between client-server
 *
 */
class FileContentMessage : public BasicMessage {

private:
    ///Vector of symbols for the previous established files
    std::vector<Symbol> symbols;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ///Classic constructor with all parameters (Type is automatically set)
    FileContentMessage(unsigned int editorId,
                       std::vector<Symbol> &symbols);

    FileContentMessage() = default;

    ///Return all the symbols
    std::vector<Symbol> &getSymbols();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

};

#endif //COOPERATIVEEDITOR_FILECONTENTMESSAGE_H
