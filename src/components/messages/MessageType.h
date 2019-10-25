#ifndef COOPERATIVEEDITOR_MESSAGETYPE_H
#define COOPERATIVEEDITOR_MESSAGETYPE_H


#include <QtCore>

///Enumeration to identify the type of the message
enum class Type : qint32 {
    CONNECT = 0, LISTING = 1, CREATE = 2, OPEN = 3, LOGIN = 4, REGISTER = 5,
    FILE_OK = 7, LOGIN_OK = 8, REGISTER_OK = 9, INSERT = 10, ERASE = 11,
    FILE_KO = 12, LOGIN_KO = 13, REGISTER_KO = 14, UNKNOWN = -1
};


#endif //COOPERATIVEEDITOR_MESSAGETYPE_H
