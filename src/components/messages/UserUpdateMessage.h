#ifndef COOPERATIVEEDITOR_USERUPDATEMESSAGE_H
#define COOPERATIVEEDITOR_USERUPDATEMESSAGE_H


#include "UserMessage.h"

/**
 * UserUpdateMessage, used when the user wants to update its info
 */
class UserUpdateMessage : public UserMessage {
private:
    QString oldPassword;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

public:
    explicit UserUpdateMessage(quint32 editorId, User user, QString oldPassword);

    UserUpdateMessage() = default;

    std::string toStdString(int level = 0) override;

    /**
     * Function to get the old password inserted
     * @return  the user old password
     */
    QString &getOldPassword();

    /**
     * Function to create a UserMessage from raw bytes
     * @param buf  the raw bytes
     * @return the parsed message
     */
    static UserUpdateMessage fromQByteArray(QByteArray &buf);
};


#endif //COOPERATIVEEDITOR_USERUPDATEMESSAGE_H
