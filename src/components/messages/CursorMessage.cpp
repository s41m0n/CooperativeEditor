#include "CursorMessage.h"
CursorMessage::CursorMessage(unsigned editorId, int position)
    : BasicMessage(editorId), position(position) {}

int CursorMessage::getPos() { return position; }

CursorMessage CursorMessage::fromQByteArray(QByteArray &buf) {
  CursorMessage msg;
  QDataStream ds(&buf, QIODevice::ReadOnly);
  ds >> msg;
  return msg;
}

std::string CursorMessage::toStdString(int level) {
  return std::string(level, '\t') + "CursorMessage{\n" +
         std::string(level + 1, '\t') +
         "editorId: " + std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') +
         "position: " + std::to_string(position) + "\n" +
         std::string(level, '\t') + "}";
}

QDataStream &CursorMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << position;
  return stream;
}
QDataStream &CursorMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> position;
  return stream;
}
