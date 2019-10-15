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

    Symbol symbol;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:

    CrdtMessage(Type msgType, Symbol symbol, unsigned editorId);

    CrdtMessage() = default;

    Symbol &getSymbol();

    std::string toStdString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_CRDTMESSAGE_H
