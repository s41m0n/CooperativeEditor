#ifndef COOPERATIVEEDITOR_BASICMESSAGE_H
#define COOPERATIVEEDITOR_BASICMESSAGE_H

#include <string>
#include <QDataStream>
#include <src/include/lib/spdlog/spdlog.h>

///Enumeration to identify the type of the message
enum class Type : qint32 {
    CONNECT = 0, LISTING = 1, CREATE = 2, OPEN = 3, LOGIN = 4, REGISTER = 5,
    FILE_OK = 7, LOGIN_OK = 8, REGISTER_OK = 9, INSERT = 10, ERASE = 11,
    FILE_KO = 12, LOGIN_KO = 13, REGISTER_KO = 14, UNKNOWN = -1
};

QDataStream &operator<<(QDataStream &stream, Type &val);

QDataStream &operator>>(QDataStream &stream, Type &val);

/**
 * BasicMessage class, represents a basic message between client-server
 *
 */
class BasicMessage {

protected:

    Type msgType;

    unsigned editorId;

    virtual void serialize(QDataStream &stream) {
      stream << msgType << editorId;
    };

    virtual void deserialize(QDataStream &stream) {
      stream >> msgType >> editorId;
    }

public:

    BasicMessage(Type msgType, unsigned editorId);

    BasicMessage();
    
    quint32 getEditorId();

    Type &getMsgType();

    virtual ///Method to print in human-readable format the message using indent
    std::string toStdString(int level = 0);

    friend QDataStream &
    operator<<(QDataStream &stream, BasicMessage &val);

    friend QDataStream &operator>>(QDataStream &stream, BasicMessage &val);

};

#endif //COOPERATIVEEDITOR_BASICMESSAGE_H
