#ifndef COOPERATIVEEDITOR_USERMESSAGE_H
#define COOPERATIVEEDITOR_USERMESSAGE_H

#include "BasicMessage.h"
#include "common/User.h"

/**
 * UserMessage class, describing all interactiong with the user structure
 * (login, logout, etc)
 */
class UserMessage : public BasicMessage {

private:
  User user;

protected:

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  explicit UserMessage(quint32 editorId, User user);

  UserMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the user structure
   * @return the user structure
   */
  User &getUser();

  /**
   * Function to create a UserMessage from raw bytes
   * @param buf  the raw bytes
   * @return the parsed message
   */
  static UserMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_USERMESSAGE_H
