#include "RequestMessage.h"

RequestMessage::RequestMessage(Type msgType, unsigned editorId,
                               QString filename) : BasicMessage(msgType,
                                                                editorId),
                                                   filename(std::move(
                                                           filename)) {
}

QString &RequestMessage::getFilename() {
  return filename;
}

std::string RequestMessage::toStdString(int level) {
  return std::string(level, '\t') + "RequestMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "file/s: " + filename.toStdString() +
         "\n" +
         std::string(level, '\t') + "}";
}

void RequestMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << filename;
}

void RequestMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> filename;
}
