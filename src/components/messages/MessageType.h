#ifndef COOPERATIVEEDITOR_MESSAGETYPE_H
#define COOPERATIVEEDITOR_MESSAGETYPE_H

#include <QtCore>

/**
 * Enumeration to identify the type of the message
 */
enum class Type : qint32 {
  U_CONNECT = 0,
  F_LISTING = 1,
  F_CREATE = 2,
  F_OPEN = 3,
  U_LOGIN = 4,
  U_REGISTER = 5,
  U_UPDATE = 6,
  F_FILE_OK = 7,
  U_LOGIN_OK = 8,
  U_REGISTER_OK = 9,
  S_INSERT = 10,
  S_ERASE = 11,
  F_FILE_KO = 12,
  U_LOGIN_KO = 13,
  U_REGISTER_KO = 14,
  U_UPDATE_OK = 15,
  U_UPDATE_KO = 16,
  U_CONNECTED = 17,
  U_DISCONNECTED = 18,
  U_GENERATE_INVITE = 19,
  U_INSERT_INVITE = 20,
  U_INSERT_INVITE_OK = 21,
  U_INSERT_INVITE_KO = 22,
  U_CURSOR = 23,
  U_UNAUTHORIZED = 24,
  UNKNOWN = -1
};

#endif // COOPERATIVEEDITOR_MESSAGETYPE_H
