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

    QVector<Symbol> symbols;

public:

    CrdtMessage(QVector<Symbol> symbols, unsigned editorId);

    CrdtMessage() = default;

    QVector<Symbol> &getSymbols();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

};


#endif //COOPERATIVEEDITOR_CRDTMESSAGE_H
