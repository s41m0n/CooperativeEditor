#ifndef COOPERATIVEEDITOR_LOGINMESSAGE_H
#define COOPERATIVEEDITOR_LOGINMESSAGE_H

#include <QString>

#include "BasicMessage.h"


/**
 * LoginMessage class to exchange login info
 */
class LoginMessage : public BasicMessage {

private:

    QString username;

    QString password;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:

    LoginMessage(unsigned editorId, QString username,
                 QString password);

    LoginMessage() = default;

    QString &getUsername();

    QString &getPassword();

    std::string toStdString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_LOGINMESSAGE_H
