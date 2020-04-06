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

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  CrdtMessage(Symbol symbols, unsigned editorId);

  CrdtMessage() = default;

  Symbol  &getSymbol();

  std::string toStdString(int level = 0) override;

  static CrdtMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_CRDTMESSAGE_H
