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
  CrdtMessage(Symbol symbols, quint32 editorId);

  CrdtMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the symbol contained in this message
   * @return the symbol
   */
  Symbol  &getSymbol();

  /**
   * Function to create a CrdtMessage from raw bytes
   * @param buf  the raw bytes
   * @return the parsed message
   */
  static CrdtMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_CRDTMESSAGE_H
