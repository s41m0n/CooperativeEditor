#include "UserUpdateMessage.h"

UserUpdateMessage::UserUpdateMessage(quint32 editorId, User user, QString oldPassword)
  : UserMessage(editorId, std::move(user)), oldPassword(std::move(oldPassword)) {}

std::string UserUpdateMessage::toStdString(int level) {
  return std::string(level, '\t') + "UserMessage{\n" +
         std::string(level + 1, '\t') +
         "editorId: " + std::to_string(editorId) + "\n" +
         getUser().toStdString(level + 1) + "\n" + std::string(level, '\t') + "}";
}

QDataStream &UserUpdateMessage::serialize(QDataStream &stream) const {
  UserMessage::serialize(stream);
  stream << oldPassword;
  return stream;
}

QDataStream &UserUpdateMessage::deserialize(QDataStream &stream) {
  UserMessage::deserialize(stream);
  stream >> oldPassword;
  return stream;
}

UserUpdateMessage UserUpdateMessage::fromQByteArray(QByteArray &buf) {
  QDataStream ds(&buf, QIODevice::ReadOnly);
  UserUpdateMessage ret;
  ds >> ret;
  return ret;
}

QString &UserUpdateMessage::getOldPassword() {
  return oldPassword;
}
