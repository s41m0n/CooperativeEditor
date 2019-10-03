#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "google-default-arguments"
//
// Created by s41m0n on 13/08/19.
//

#include "Message.h"

//BASIC MESSAGE
BasicMessage::BasicMessage(Type msgType, unsigned int editorId) : editorId(
        editorId), msgType(msgType) {
}

BasicMessage::BasicMessage(const BasicMessage &msg) : BasicMessage(msg.msgType,
                                                                   msg.editorId) {
}
#pragma clang diagnostic pop

BasicMessage::BasicMessage(BasicMessage &&msg) noexcept {
  editorId = msg.editorId;
  msgType = msg.msgType;
}

BasicMessage::BasicMessage() : editorId(0), msgType(Type::UNKNOWN) {
}


unsigned int BasicMessage::getEditorId() {
  return editorId;
}

Type BasicMessage::getMsgType() {
  return msgType;
}

std::string BasicMessage::toString(int level) {
  return std::string(level, '\t') + "BasicMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level, '\t') + "}";
}

std::string BasicMessage::toString() {
  return toString(0);
}

//CRDT MESSAGE
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

std::string CrdtMessage::toString() {
  return toString(0);
}

//REQUEST MESSAGE
RequestMessage::RequestMessage(Type msgType, unsigned int editorId,
                               std::string &filename) : BasicMessage(msgType,
                                                                     editorId),
                                                        filename(filename) {
}

std::string RequestMessage::getFilename() {
  return filename;
}

RequestMessage::RequestMessage(BasicMessage &&msg) : BasicMessage(
        std::move(msg)) {
}

std::string RequestMessage::toString(int level) {
  return std::string(level, '\t') + "RequestMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "file/s: " + filename + "\n" +
         std::string(level, '\t') + "}";
}

std::string RequestMessage::toString() {
  return toString(0);
}

//FILE CONTENT MESSAGE
FileContentMessage::FileContentMessage(Type msgType, unsigned int editorId,
                                       std::vector<Symbol> &symbols)
        : BasicMessage(msgType, editorId), symbols(symbols) {
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

std::string FileContentMessage::toString() {
  return toString(0);
}