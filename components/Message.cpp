//
// Created by s41m0n on 16/05/19.
//

#include "Message.h"

Message::Message(Type msgType, Symbol symbol, int editorId)
        : symbol(std::move(symbol)),msgType(msgType), editorId(editorId){}


Message::Message() : editorId(-1), msgType(UNKNOWN), symbol(){}

Message::Message(Type msgType, int editorId) : msgType(msgType), symbol(), editorId(editorId){}

const int Message::getEditorId() const {
    return this->editorId;
}

const Type Message::getMsgType() {
    return this->msgType;
}

const Symbol Message::getSymbol() {
    return this->symbol;
}
