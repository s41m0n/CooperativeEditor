//
// Created by s41m0n on 4/8/20.
//

#ifndef COOPERATIVEEDITOR_CURSORMESSAGE_H
#define COOPERATIVEEDITOR_CURSORMESSAGE_H

#include <string>
#include "BasicMessage.h"

class CursorMessage : public BasicMessage{
private:
  int position = -1;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;


public:
  CursorMessage(unsigned editorId, int position);

  CursorMessage() = default;

  int getPos();

  std::string toStdString(int level = 0) override;

  static CursorMessage fromQByteArray(QByteArray &buf);

};

#endif // COOPERATIVEEDITOR_CURSORMESSAGE_H
