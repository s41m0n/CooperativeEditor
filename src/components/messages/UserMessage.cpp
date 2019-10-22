#include "UserMessage.h"

#include <utility>

User &UserMessage::getUser() {
  return user;
}

UserMessage::UserMessage(Type type, unsigned editorId, User user)
        : BasicMessage(type, editorId), user(std::move(user)) {
}

void UserMessage::serialize(QDataStream &stream) {
  BasicMessage::serialize(stream);
  stream << user;
}

void UserMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> user;
}

std::string UserMessage::toStdString(int level) {
  return std::string(level, '\t') + "UserMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         user.toStdString(level + 1) + "\n" +
         std::string(level, '\t') + "}";
}
