#include "FileContentMessage.h"

FileContentMessage::FileContentMessage(unsigned editorId,
                                       QVector<Symbol> &symbols)
        : BasicMessage(Type::CONTENT, editorId), symbols(symbols) {
}

QVector<Symbol> &FileContentMessage::getSymbols() {
  return symbols;
}

std::string FileContentMessage::toStdString(int level) {
  std::string tmp;
  tmp += std::string(level, '\t') + "FileContentMessage{\n";
  tmp += std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n";
  tmp += std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n";
  tmp += std::string(level + 1, '\t') + "content: [\n";
  for (auto &s: symbols)
    tmp += s.toStdString(level + 2) + "\n";
  tmp += std::string(level + 1, '\t') + "]\n" + std::string(level, '\t') + "}";
  return tmp;
}

void FileContentMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << symbols;
}

void FileContentMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> symbols;
}
