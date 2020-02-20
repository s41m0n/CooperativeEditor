#ifndef COOPERATIVEEDITOR_MESSAGETYPE_H
#define COOPERATIVEEDITOR_MESSAGETYPE_H

#include <QtCore>

/// Enumeration to identify the type of the message
enum class Type : qint32 {
  CONNECT = 0,
  LISTING = 1,
  CREATE = 2,
  OPEN = 3,
  LOGIN = 4,
  REGISTER = 5,
  UNREGISTER = 6,
  UPDATE = 7,
  FILE_OK = 8,
  LOGIN_OK = 9,
  REGISTER_OK = 10,
  INSERT = 11,
  ERASE = 12,
  FILE_KO = 13,
  LOGIN_KO = 14,
  REGISTER_KO = 15,
  UNREGISTER_OK = 16,
  UNREGISTER_KO = 17,
  UPDATE_OK = 18,
  UPDATE_KO = 19,
  UNKNOWN = -1
};

#endif // COOPERATIVEEDITOR_MESSAGETYPE_H
