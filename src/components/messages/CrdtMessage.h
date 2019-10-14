#ifndef COOPERATIVEEDITOR_CRDTMESSAGE_H
#define COOPERATIVEEDITOR_CRDTMESSAGE_H

#include "BasicMessage.h"
#include "components/Symbol.h"

/**
 * CrdtMessage class, represents a Symbol-exchange message between client-server
 *
 */
class CrdtMessage : public BasicMessage {

private:
    ///The symbol contained in the message (can be empty)
    Symbol symbol;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ///Classic constructor with all parameters
    CrdtMessage(Type msgType, Symbol symbol, unsigned int editorId);

    ///Constructor used to create a message to be filled
    CrdtMessage() = default;

    ///Return the Symbol contained in the Message
    Symbol &getSymbol();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_CRDTMESSAGE_H
