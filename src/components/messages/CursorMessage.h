//
// Created by s41m0n on 4/8/20.
//

#ifndef COOPERATIVEEDITOR_CURSORMESSAGE_H
#define COOPERATIVEEDITOR_CURSORMESSAGE_H

#include "BasicMessage.h"
#include <string>

/**
 * CursorMessage class, to represent a message containing a cursor movement
 */
class CursorMessage : public BasicMessage {
private:
  quint32 position = 0;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  CursorMessage(quint32 editorId, quint32 position);

  CursorMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the position of the cursor
   * @return  the position
   */
  quint32 getPos();

  /**
   * Function to create a CursorMessage from raw bytes
   * @param buf  the raw bytes
   * @return the parsed message
   */
  static CursorMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_CURSORMESSAGE_H
