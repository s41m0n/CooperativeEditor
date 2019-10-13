//
// Created by s41m0n on 20/05/19.
//

#ifndef COOPERATIVEEDITOR_MESSAGE_H
#define COOPERATIVEEDITOR_MESSAGE_H


#include <spdlog/spdlog.h>
#include <string>
#include <utility>

#include "components/Symbol.h"

///Enumeration to identify the type of the message
enum class Type {
    CONNECT = 0, LISTING = 1, CREATE = 2, OPEN = 3, FILEOK = 4, FILEKO = 5,
    CONTENT = 6, INSERT = 7, ERASE = 8, UNKNOWN = -1,
};

/**
 * BasicMessage class, represents a basic message between client-server
 *
 * @author Simone Magnani - s41m0n
 */
class BasicMessage {

protected:
    ///The type of the message
    Type msgType;

    ///The editor specific ID
    unsigned int editorId;

public:
    ///Constructor used in case of Connect Message
    BasicMessage(Type msgType, unsigned int editorId);

    ///Move constructor
    BasicMessage(BasicMessage &&msg) noexcept;

    ///Constructor used to create a message to be filled
    BasicMessage();

    ///Return the editor ID
    unsigned int getEditorId();

    ///Return the type of the Message
    Type getMsgType();

    ///Method to print in human-readable format the message
    virtual std::string toString(int level);

    virtual std::string toString();

    ///Operator overload '<<' for BasicMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const BasicMessage &val) {
      stream << static_cast<quint32>(val.msgType) << val.editorId;
      return stream;
    }

    ///Operator overload '>>' for BasicMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, BasicMessage &val) {
      stream >> reinterpret_cast<quint32 &>(val.msgType) >> val.editorId;
      return stream;
    };
};

/**
 * RequestMessage class, represents some initial setup messages between client-server
 *
 * @author Simone Magnani - s41m0n
 */
class RequestMessage : public BasicMessage {

private:
    ///The name of the requested file
    std::string filename;

public:
    ///Classic constructor with all parameters given
    RequestMessage(Type msgType, unsigned int editorId, std::string &filename);

    ///Method build a RequestMessage from a BasicMessage
    explicit RequestMessage(BasicMessage &&msg);

    ///Return the requested filename
    std::string getFilename();

    ///Method to print in human-readable format the message
    std::string toString(int level) override;

    std::string toString() override;

    ///Operator overload '<<' for RequestMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const RequestMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << val.filename.c_str();
      return stream;
    }

    ///Operator overload '>>' for RequestMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, RequestMessage &val) {
      char *tmp;
      stream >> tmp;
      if (tmp)
        val.filename = std::string(tmp);
      delete[] tmp;
      return stream;
    };
};

/**
 * FileContentMessage class, represents a File content transfer message between client-server
 *
 * @author Simone Magnani - s41m0n
 */
class FileContentMessage : public BasicMessage {

private:
    ///Vector of symbols for the previous established files
    std::vector<Symbol> symbols;

public:
    ///Classic constructor with all parameters
    FileContentMessage(Type msgType, unsigned int editorId,
                       std::vector<Symbol> &symbols);

    ///Method build a FileContentMessage from a BasicMessage
    explicit FileContentMessage(BasicMessage &&msg);

    ///Return all the symbols
    std::vector<Symbol> &getSymbols();

    ///Method to print in human-readable format the message
    std::string toString(int level) override;

    std::string toString() override;

    ///Operator overload '<<' for FileContentMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const FileContentMessage &val) {
      stream << static_cast<const BasicMessage &>(val)
             << static_cast<quint32>(val.symbols.size());
      for (auto &tmp: val.symbols)
        stream << tmp;
      return stream;
    }

    ///Operator overload '>>' for FileContentMessage when using QDataStream for serialization
    friend QDataStream &
    operator>>(QDataStream &stream, FileContentMessage &val) {
      quint32 size;
      stream >> size;
      for (quint32 i = 0; i < size; i++) {
        Symbol tmp;
        stream >> tmp;
        val.symbols.emplace_back(tmp);
      }
      return stream;
    };

};

/**
 * CrdtMessage class, represents a Symbol-exchange message between client-server
 *
 * @author Simone Magnani - s41m0n
 */
class CrdtMessage : public BasicMessage {

private:
    ///The symbol contained in the message (can be empty)
    Symbol symbol;

public:
    ///Classic constructor with all parameters
    CrdtMessage(Type msgType, Symbol symbol, unsigned int editorId);

    ///Method build a CrdtMessage from a BasicMessage
    explicit CrdtMessage(BasicMessage &&msg);

    ///Constructor used to create a message to be filled
    CrdtMessage() = default;

    ///Return the Symbol contained in the Message
    Symbol &getSymbol();

    ///Method to print in human-readable format the message
    std::string toString(int level) override;

    std::string toString() override;

    ///Operator overload '<<' for CrdtMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const CrdtMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << val.symbol;
      return stream;
    }

    ///Operator overload '>>' for CrdtMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, CrdtMessage &val) {
      stream >> val.symbol;
      return stream;
    };
};

#endif //COOPERATIVEEDITOR_MESSAGE_H