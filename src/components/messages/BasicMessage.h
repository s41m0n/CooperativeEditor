#ifndef COOPERATIVEEDITOR_BASICMESSAGE_H
#define COOPERATIVEEDITOR_BASICMESSAGE_H

#include "src/include/lib/serialization/Serializable.h"

/**
 * BasicMessage class, represents a basic message between client-server
 *
 */
class BasicMessage : public Serializable {

protected:
  unsigned editorId;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  BasicMessage(unsigned editorId);

  BasicMessage();

  quint32 getEditorId();

  std::string toStdString(int level = 0) override;

  static BasicMessage fromQByteArray(QByteArray &buf);

  static QByteArray toQByteArray(BasicMessage &msg);
};

#endif // COOPERATIVEEDITOR_BASICMESSAGE_H
