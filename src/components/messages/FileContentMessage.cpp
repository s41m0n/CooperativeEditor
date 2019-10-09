#include "FileContentMessage.h"

FileContentMessage::FileContentMessage(unsigned int editorId,
                                       std::vector<Symbol> &symbols)
        : BasicMessage(Type::CONTENT, editorId), symbols(symbols) {
}

FileContentMessage::FileContentMessage(BasicMessage &&msg) : BasicMessage(
        std::move(msg)) {
}

std::vector<Symbol> &FileContentMessage::getSymbols() {
  return symbols;
}

std::string FileContentMessage::toString(int level) {
  std::string tmp;
  tmp += std::string(level, '\t') + "FileContentMessage{\n";
  tmp += std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n";
  tmp += std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n";
  tmp += std::string(level + 1, '\t') + "content: [\n";
  for (auto &s: symbols)
    tmp += s.toString(level + 2) + "\n";
  tmp += std::string(level + 1, '\t') + "]\n" + std::string(level, '\t') + "}";
  return tmp;
}
