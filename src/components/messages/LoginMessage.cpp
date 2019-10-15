#include "LoginMessage.h"

LoginMessage::LoginMessage(unsigned editorId, QString username,
                           QString password) : BasicMessage(Type::LOGIN,
                                                            editorId),
                                               username(
                                                       std::move(username)),
                                               password(std::move(
                                                       password)) {
}

QString &LoginMessage::getUsername() {
  return username;
}

QString &LoginMessage::getPassword() {
  return password;
}

std::string LoginMessage::toStdString(int level) {
  return std::string(level, '\t') + "LoginMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "username: " +
         username.toStdString() + "\n" +
         std::string(level + 1, '\t') + "password: " +
         password.toStdString() + "\n" +
         std::string(level, '\t') + "}";
}

void LoginMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << username << password;
}

void LoginMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> username >> password;
}
