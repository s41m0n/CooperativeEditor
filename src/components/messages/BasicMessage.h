#ifndef COOPERATIVEEDITOR_BASICMESSAGE_H
#define COOPERATIVEEDITOR_BASICMESSAGE_H

#include <string>
#include <QDataStream>
#include <components/Symbol.h>

///Enumeration to identify the type of the message
enum class Type {
    CONNECT = 0, LISTING = 1, CREATE = 2, OPEN = 3, LOGIN = 4,
    FILE_RESULT = 5, LOGIN_RESULT = 6,
    CONTENT = 7, INSERT = 8, ERASE = 9, UNKNOWN = -1,
};

/**
 * BasicMessage class, represents a basic message between client-server
 *
 */
class BasicMessage {

protected:
    ///The type of the message
    Type msgType;

    ///The editor specific ID
    unsigned int editorId;

    virtual void serialize(QDataStream &stream){
      stream << static_cast<quint32>(msgType) << editorId;
    };

    virtual void deserialize(QDataStream &stream) {
      stream >> reinterpret_cast<quint32 &>(msgType) >> editorId;
    }

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

    virtual ///Method to print in human-readable format the message using indent
    std::string toString(int level = 0);

    ///Operator overload '<<' for BasicMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, BasicMessage &val);

    ///Operator overload '>>' for BasicMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, BasicMessage &val);

};


#endif //COOPERATIVEEDITOR_BASICMESSAGE_H
