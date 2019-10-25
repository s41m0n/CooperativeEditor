#include "BasicMessage.h"

BasicMessage::BasicMessage(Type msgType, unsigned editorId) : editorId(
        editorId), msgType(msgType) {
}

BasicMessage::BasicMessage() : editorId(0),
                               msgType(Type::UNKNOWN) {
}


unsigned int BasicMessage::getEditorId() {
  return editorId;
}

Type &BasicMessage::getMsgType() {
  return msgType;
}

std::string BasicMessage::toStdString(int level) {
  return std::string(level, '\t') + "BasicMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level, '\t') + "}";
}

QDataStream &BasicMessage::serialize(QDataStream &stream) const {
  stream << editorId << msgType;
  return stream;
}

QDataStream &BasicMessage::deserialize(QDataStream &stream) {
  stream >> editorId >> msgType;
  return stream;
}
