#include "FileContentMessage.h"

FileContentMessage::FileContentMessage(unsigned int editorId,
                                       std::vector<Symbol> &symbols)
        : BasicMessage(Type::CONTENT, editorId), symbols(symbols) {
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

void FileContentMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << static_cast<quint32>(symbols.size());
  for (auto &tmp: symbols)
    stream << tmp;
}

void FileContentMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  quint32 size;
  stream >> size;
  Symbol tmp;
  for (quint32 i = 0; i < size; i++) {
    stream >> tmp;
    symbols.emplace_back(tmp);
  }
}
