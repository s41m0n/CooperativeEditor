//
// Created by s41m0n on 10/10/19.
//

#include "FileListingMessage.h"

FileListingMessage::FileListingMessage(unsigned int editorId,
                                       std::vector<std::string> &files)
        : BasicMessage(Type::LISTING, editorId), files(files) {
}

FileListingMessage::FileListingMessage(BasicMessage &&msg) : BasicMessage(
        std::move(msg)) {
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
