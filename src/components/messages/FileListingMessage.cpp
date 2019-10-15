#include "FileListingMessage.h"

FileListingMessage::FileListingMessage(unsigned editorId,
                                       QVector<QString> &files)
        : BasicMessage(Type::LISTING, editorId), files(files) {
}

QVector<QString> &FileListingMessage::getFiles() {
  return files;
}

std::string FileListingMessage::toStdString(int level) {
  std::string tmp;
  tmp += std::string(level, '\t') + "FileListingMessage{\n";
  tmp += std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n";
  tmp += std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n";
  tmp += std::string(level + 1, '\t') + "files: [\n";
  for (auto &s: files)
    tmp += std::string(level + 1, '\t') + s.toStdString() + "\n";
  tmp += std::string(level + 1, '\t') + "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

void FileListingMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << files;
}

void FileListingMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> files;
}
