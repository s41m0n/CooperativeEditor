#include "CrdtMessage.h"

CrdtMessage::CrdtMessage(QVector<Symbol> symbols, unsigned editorId)
        : BasicMessage(editorId),
          symbols(std::move(symbols)) {
}

QVector<Symbol> &CrdtMessage::getSymbols() {
  return symbols;
}

std::string CrdtMessage::toStdString(int level) {
  return std::string(level, '\t') + "CrdtMessage{\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         symbols[0].toStdString(level + 1) + "\n" +
         std::string(level, '\t') + "}";
}

QDataStream &CrdtMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << symbols;
  return stream;
}

QDataStream &CrdtMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> symbols;
  return stream;
}
