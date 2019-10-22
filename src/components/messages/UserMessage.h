#ifndef COOPERATIVEEDITOR_USERMESSAGE_H
#define COOPERATIVEEDITOR_USERMESSAGE_H

#include "BasicMessage.h"
#include "common/User.h"

class UserMessage : public BasicMessage{

private:
    User user;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    explicit UserMessage(Type type, unsigned editorId, User user);

    UserMessage() = default;

    User &getUser();

    std::string toStdString(int level = 0) override;
};


#endif //COOPERATIVEEDITOR_USERMESSAGE_H
