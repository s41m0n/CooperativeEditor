//
// Created by s41m0n on 10/10/19.
//

#include "FileListingMessage.h"

FileListingMessage::FileListingMessage(unsigned int editorId,
                                       std::vector<std::string> &files)
        : BasicMessage(Type::LISTING, editorId), files(files) {
}

std::vector<std::string> &FileListingMessage::getFiles() {
  return files;
}

std::string FileListingMessage::toString(int level) {
  std::string tmp;
  tmp += std::string(level, '\t') + "FileListingMessage{\n";
  tmp += std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n";
  tmp += std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n";
  tmp += std::string(level + 1, '\t') + "files: [\n";
  for (auto &s: files)
    tmp += std::string(level + 1, '\t') + s + "\n";
  tmp += std::string(level + 1, '\t') + "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

void FileListingMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << static_cast<quint32>(files.size());
  for (const auto &v : files) {
    stream << v.c_str();
  }
}

void FileListingMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  quint32 length;
  stream >> length;
  for (quint32 i = 0; i < length; ++i) {
    char *tmp;
    stream >> tmp;
    if (tmp)
      files.emplace_back(tmp);
    delete[] tmp;
  }
}
