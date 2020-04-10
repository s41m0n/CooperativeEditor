#include "FileListingMessage.h"

#include <utility>

FileListingMessage::FileListingMessage(quint32 editorId, QVector<QString> files)
    : BasicMessage(editorId), files(std::move(files)) {}

QVector<QString> &FileListingMessage::getFiles() { return files; }

std::string FileListingMessage::toStdString(int level) {
  std::string tmp;
  tmp += std::string(level, '\t') + "FileListingMessage{\n";
  tmp += std::string(level + 1, '\t') +
         "editorId: " + std::to_string(editorId) + "\n";
  tmp += std::string(level + 1, '\t') + "files: [\n";
  for (auto &s : files)
    tmp += std::string(level + 1, '\t') + s.toStdString() + "\n";
  tmp += std::string(level + 1, '\t') + "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

QDataStream &FileListingMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << files;
  return stream;
}

QDataStream &FileListingMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> files;
  return stream;
}

FileListingMessage FileListingMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  FileListingMessage ret;
  ds >> ret;
  return ret;
}