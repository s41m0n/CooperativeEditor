#include "CrdtMessage.h"

CrdtMessage::CrdtMessage(Type msgType, Symbol symbol, unsigned int editorId)
        : BasicMessage(msgType, editorId),
          symbol(std::move(symbol)) {
}

CrdtMessage::CrdtMessage(BasicMessage &&msg) : BasicMessage(std::move(msg)) {
}

Symbol &CrdtMessage::getSymbol() {
  return symbol;
}

std::string CrdtMessage::toString(int level) {
  return std::string(level, '\t') + "CrdtMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         symbol.toString(level + 1) + "\n" +
         std::string(level, '\t') + "}";
}
