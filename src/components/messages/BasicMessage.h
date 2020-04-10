#ifndef COOPERATIVEEDITOR_BASICMESSAGE_H
#define COOPERATIVEEDITOR_BASICMESSAGE_H

#include "src/include/lib/serialization/Serializable.h"
#include <QByteArray>

/**
 * BasicMessage class, represents a basic message between client-server
 *
 */
class BasicMessage : public Serializable {

protected:
  quint32 editorId{};

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  explicit BasicMessage(quint32 editorId);

  BasicMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the editor id of this message
   * @return  the editor id
   */
  quint32 getEditorId();

  /**
   * Function to create a BasicMessage from raw data
   * @param buf the buffer containing raw bytes
   * @return the parsed message
   */
  static BasicMessage fromQByteArray(QByteArray &buf);

  /**
   * Function to convert a BasicMessage to raw bytes
   * @param msg the message to be converted
   * @return the raw bytes
   */
  static QByteArray toQByteArray(BasicMessage &msg);
};

#endif // COOPERATIVEEDITOR_BASICMESSAGE_H
