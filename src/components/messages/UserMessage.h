#ifndef COOPERATIVEEDITOR_USERMESSAGE_H
#define COOPERATIVEEDITOR_USERMESSAGE_H

#include "BasicMessage.h"
#include "common/User.h"

class UserMessage : public BasicMessage {

private:
  User user;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  explicit UserMessage(unsigned editorId, User user);

  UserMessage() = default;

  User &getUser();

  std::string toStdString(int level = 0) override;

  static UserMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_USERMESSAGE_H
