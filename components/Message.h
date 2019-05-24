//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_MESSAGE_H
#define COOPERATIVEEDITOR_MESSAGE_H


#include "Symbol.h"


// Forward declaration of class boost::serialization::access
#pragma once
namespace boost {
    namespace serialization {
        class access;
    }
}

///Enumeration to identify the type of the message
enum Type {
    ///Connect message, sent by the server to client with his new ID
    CONNECT,
    ///Insert message, to notify the insertion of a new symbol
    INSERT,
    ///Erase message, to notify the deletion of a symbol
    ERASE,
    ///Unknown message, used when created a non-valid Message
    UNKNOWN
};

/**
 * Message class, represents the messages between client-server
 *
 * @author Simone Magnani - s41m0n
 */
class Message {

private:
    ///The editor specific ID
    int editorId;
    ///The type of the message
    Type msgType;
    ///The symbol contained in the message (can be empty)
    Symbol symbol;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;

    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & editorId & msgType & symbol;
    }

public:
    ///Classic constructor with all parameters
    Message(Type msgType, Symbol symbol, int editorId);
    ///Constructor used in case of Connect Message
    Message(Type msgType, int editorId);
    ///Constructor used to create a non-valid message to be filled
    Message();
    ///Return the editor ID
    const int getEditorId() const;
    ///Return the type of the Message
    const Type getMsgType();
    ///Return the Symbol contained in the Message
    const Symbol getSymbol();

};



#endif //COOPERATIVEEDITOR_MESSAGE_H
