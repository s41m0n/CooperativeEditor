#include "BasicMessage.h"

BasicMessage::BasicMessage(unsigned editorId) : editorId(editorId) {}

BasicMessage::BasicMessage() : editorId(0) {}

unsigned int BasicMessage::getEditorId() {
  return editorId;
}

std::string BasicMessage::toStdString(int level) {
  return std::string(level, '\t') + "BasicMessage{\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level, '\t') + "}";
}

QDataStream &BasicMessage::serialize(QDataStream &stream) const {
  stream << editorId;
  return stream;
}

QDataStream &BasicMessage::deserialize(QDataStream &stream) {
  stream >> editorId;
  return stream;
}

BasicMessage BasicMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  BasicMessage ret;
  ds >> ret;
  return ret;
}

QByteArray BasicMessage::toQByteArray(BasicMessage &msg) {
  QByteArray buf;
  QDataStream ds(&buf, QIODevice::WriteOnly);
  ds << msg;
  return buf;
}