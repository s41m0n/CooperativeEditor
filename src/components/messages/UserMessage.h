#ifndef COOPERATIVEEDITOR_USERMESSAGE_H
#define COOPERATIVEEDITOR_USERMESSAGE_H

#include "BasicMessage.h"
#include "common/User.h"

class UserMessage : public BasicMessage {

private:
    
    User user;

public:

    explicit UserMessage(Type type, unsigned editorId, User user);

    UserMessage() = default;

    User &getUser();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

    quint32 objectSize() override;

};


#endif //COOPERATIVEEDITOR_USERMESSAGE_H
