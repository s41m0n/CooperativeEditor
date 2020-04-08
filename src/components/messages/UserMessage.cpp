#include "UserMessage.h"

UserMessage::UserMessage(quint32 editorId, User user)
    : BasicMessage(editorId), user(std::move(user)) {}

User &UserMessage::getUser() { return user; }

std::string UserMessage::toStdString(int level) {
  return std::string(level, '\t') + "UserMessage{\n" +
         std::string(level + 1, '\t') +
         "editorId: " + std::to_string(editorId) + "\n" +
         user.toStdString(level + 1) + "\n" + std::string(level, '\t') + "}";
}

QDataStream &UserMessage::serialize(QDataStream &stream) const {
  BasicMessage::serialize(stream);
  stream << user;
  return stream;
}

QDataStream &UserMessage::deserialize(QDataStream &stream) {
  BasicMessage::deserialize(stream);
  stream >> user;
  return stream;
}

UserMessage UserMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  UserMessage ret;
  ds >> ret;
  return ret;
}