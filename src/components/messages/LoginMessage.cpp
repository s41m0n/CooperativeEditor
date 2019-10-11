#include "LoginMessage.h"

LoginMessage::LoginMessage(unsigned editorId, std::string username,
                           std::string password) : BasicMessage(Type::LOGIN,
                                                                editorId),
                                                   username(
                                                           std::move(username)),
                                                   password(std::move(
                                                           password)) {
}

LoginMessage::LoginMessage(BasicMessage &&msg) : BasicMessage(std::move(msg)) {
}

std::string &LoginMessage::getUsername() {
  return username;
}

std::string &LoginMessage::getPassword() {
  return password;
}

std::string LoginMessage::toString(int level) {
  return std::string(level, '\t') + "LoginMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "username: " +
         username + "\n" +
         std::string(level + 1, '\t') + "password: " +
         password + "\n" +
         std::string(level, '\t') + "}";
}
