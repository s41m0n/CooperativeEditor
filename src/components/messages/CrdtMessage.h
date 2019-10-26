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

public:

    CrdtMessage(Type msgType, Symbol symbol, unsigned editorId);

    CrdtMessage() = default;

    Symbol &getSymbol();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

    quint32 objectSize() override;

};


#endif //COOPERATIVEEDITOR_CRDTMESSAGE_H
