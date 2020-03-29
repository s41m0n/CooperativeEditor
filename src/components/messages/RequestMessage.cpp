#include "RequestMessage.h"

RequestMessage::RequestMessage(unsigned editorId, QString filename)
        : BasicMessage(editorId),
          filename(std::move(
                  filename)) {
}

QString &RequestMessage::getFilename() {
  return filename;
}

std::string RequestMessage::toStdString(int level) {
  return std::string(level, '\t') + "RequestMessage{\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "file/s: " + filename.toStdString() +
         "\n" +
         std::string(level, '\t') + "}";
}

QDataStream &RequestMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << filename;
  return stream;
}

QDataStream &RequestMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> filename;
  return stream;
}


RequestMessage RequestMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  RequestMessage ret;
  ds >> ret;
  return ret;
}