#include "ResultMessage.h"

ResultMessage::ResultMessage(Type msgType, unsigned editorId, bool result)
        : BasicMessage(msgType, editorId), result(result) {
}

bool ResultMessage::isPositive() {
  return result;
}

std::string ResultMessage::toString(int level) {
  return std::string(level, '\t') + "ResultMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "result: " +
         std::to_string(result) + "\n" +
         std::string(level, '\t') + "}";
}

void ResultMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << result;
}

void ResultMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> result;
}
