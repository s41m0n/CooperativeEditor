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

public:
    ///Classic constructor with all parameters
    CrdtMessage(Type msgType, Symbol symbol, unsigned int editorId);

    ///Method build a CrdtMessage from a BasicMessage
    explicit CrdtMessage(BasicMessage &&msg);

    ///Constructor used to create a message to be filled
    CrdtMessage() = default;

    ///Return the Symbol contained in the Message
    Symbol &getSymbol();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

    ///Operator overload '<<' for CrdtMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const CrdtMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << val.symbol;
      return stream;
    }

    ///Operator overload '>>' for CrdtMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, CrdtMessage &val) {
      stream >> val.symbol;
      return stream;
    };
};


#endif //COOPERATIVEEDITOR_CRDTMESSAGE_H
