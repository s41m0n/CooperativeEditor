#include "CrdtMessage.h"

CrdtMessage::CrdtMessage(Type msgType, Symbol symbol, unsigned editorId)
        : BasicMessage(msgType, editorId),
          symbol(std::move(symbol)) {
}

Symbol &CrdtMessage::getSymbol() {
  return symbol;
}

std::string CrdtMessage::toStdString(int level) {
  return std::string(level, '\t') + "CrdtMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         symbol.toStdString(level + 1) + "\n" +
         std::string(level, '\t') + "}";
}

void CrdtMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << symbol;
}

void CrdtMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> symbol;
}
