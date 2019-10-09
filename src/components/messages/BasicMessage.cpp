#include "BasicMessage.h"

BasicMessage::BasicMessage(Type msgType, unsigned int editorId) : editorId(
        editorId), msgType(msgType) {
}

BasicMessage::BasicMessage(BasicMessage &&msg) noexcept {
  editorId = msg.editorId;
  msgType = msg.msgType;
}

BasicMessage::BasicMessage() : editorId(0), msgType(Type::UNKNOWN) {
}


unsigned int BasicMessage::getEditorId() {
  return editorId;
}

Type BasicMessage::getMsgType() {
  return msgType;
}

std::string BasicMessage::toString(int level) {
  return std::string(level, '\t') + "BasicMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level, '\t') + "}";
}
