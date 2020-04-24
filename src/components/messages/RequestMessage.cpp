#include "RequestMessage.h"

RequestMessage::RequestMessage(quint32 editorId, QString resource)
    : BasicMessage(editorId), resource(std::move(resource)) {}

QString &RequestMessage::getResource() { return resource; }

std::string RequestMessage::toStdString(int level) {
  return std::string(level, '\t') + "RequestMessage{\n" +
         std::string(level + 1, '\t') +
         "editorId: " + std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "resource: " + resource.toStdString() +
         "\n" + std::string(level, '\t') + "}";
}

QDataStream &RequestMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << resource;
  return stream;
}

QDataStream &RequestMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> resource;
  return stream;
}

RequestMessage RequestMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  RequestMessage ret;
  ds >> ret;
  return ret;
}