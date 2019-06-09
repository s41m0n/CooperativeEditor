//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_MESSAGE_H
#define COOPERATIVEEDITOR_MESSAGE_H


#include <spdlog/spdlog.h>
#include <set>
#include <string>
#include <boost/serialization/set.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/level_enum.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "Symbol.h"

// Forward declaration of class boost::serialization::access
#pragma once
namespace boost::serialization {
    class access;
}

///Enumeration to identify the type of the message
enum Type {
    CONNECT, LISTING, FILEOK, FILEKO, CONTENT,
    CREATE, OPEN, INSERT, ERASE,
    UNKNOWN
};

/**
 * Message class, represents the messages between client-server
 *
 * @author Simone Magnani - s41m0n
 */
class BasicMessage {

private:
    ///The editor specific ID
    unsigned int editorId;
    ///The type of the message
    Type msgType;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;
    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & editorId;
        ar & msgType;
    }

public:
    ///Constructor used in case of Connect Message
    BasicMessage(Type msgType, unsigned int editorId) : editorId(editorId), msgType(msgType){};
    ///Constructor used to create a message to be filled
    BasicMessage() : editorId(0), msgType(UNKNOWN){};
    virtual ~BasicMessage() = default;
    ///Return the editor ID
    const int getEditorId() const {return editorId;};
    ///Return the type of the Message
    const Type getMsgType() {return msgType;};

};

class CrdtMessage : public BasicMessage{
private:
    ///The symbol contained in the message (can be empty)
    Symbol symbol;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;
    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & boost::serialization::base_object<BasicMessage>(*this);
        ar & symbol;
    }

public:
    ///Classic constructor with all parameters
    CrdtMessage (Type msgType, Symbol& symbol, int editorId) : BasicMessage(msgType, editorId), symbol(symbol){};
    ///Constructor used to create a message to be filled
    CrdtMessage () = default;
    ///Return the Symbol contained in the Message
    const Symbol getSymbol() {return symbol;};

};

class RequestMessage : public BasicMessage {

private:
    ///The name of the requested file
    std::string filename;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;
    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & boost::serialization::base_object<BasicMessage>(*this);
        ar & filename;
    }

public:
    ///Constructor used to create a message to be filled
    RequestMessage() = default;
    ///Classic constructor with all parameters given
    RequestMessage(Type msgType, int editorId, std::string& filename) : BasicMessage(msgType, editorId), filename(filename){};
    ///Return the requested filename
    const std::string getFilename() {return filename;};
};

class FilesListingMessage : public BasicMessage {

private:
    ///The set of available files in the server
    std::string files;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;
    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & boost::serialization::base_object<BasicMessage>(*this);
        ar & files;
    }

public:
    ///Constructor used to create a message to be filled
    FilesListingMessage() = default;
    ///Classic constructor with all parameters
    FilesListingMessage(Type msgType, int editorId, std::string& files) : BasicMessage(msgType, editorId), files(files){};
    ///Return the set of available files
    const std::string getFiles() {return files;};

};

class FileContentMessage : public BasicMessage{

private:
    ///Vector of symbols for the previous established files
    std::vector<Symbol> symbols;
    ///Class to access Boost to serialize this class
    friend class boost::serialization::access;
    ///Method called by Boost to serialize
    template<typename Archive>
    void serialize(Archive& ar, const unsigned version) {
        ar & boost::serialization::base_object<BasicMessage>(*this);
        ar & symbols;
    }

public:
    ///Constructor used to create a message to be filled
    FileContentMessage() = default;
    ///Classic constructor with all parameters
    FileContentMessage(Type msgType, int editorId, std::vector<Symbol>& symbols) : BasicMessage(msgType, editorId), symbols(symbols){};
    ///Return all the symbols
    const std::vector<Symbol> getSymbols() {return symbols;};
};

#endif //COOPERATIVEEDITOR_MESSAGE_H
