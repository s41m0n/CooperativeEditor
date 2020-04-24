#include "FileMessage.h"

FileMessage::FileMessage(quint32 editorId, File &file)
        : BasicMessage(editorId), file(file) {
}

File &FileMessage::getFile() {
  return file;
}

std::string FileMessage::toStdString(int level) {
  std::string tmp;
  tmp += std::string(level, '\t') + "FileMessage{\n";
  tmp += std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n";
  tmp += std::string(level + 1, '\t') + "content: [\n";
  for (auto &s: file.getFileText())
    tmp += s.toStdString(level + 2) + "\n";
  tmp += std::string(level + 1, '\t') + "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

QDataStream &FileMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << file;
  return stream;
}

QDataStream &FileMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> file;
  return stream;
}


FileMessage FileMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  FileMessage ret;
  ds >> ret;
  return ret;
}