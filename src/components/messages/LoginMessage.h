#ifndef COOPERATIVEEDITOR_LOGINMESSAGE_H
#define COOPERATIVEEDITOR_LOGINMESSAGE_H

#include "BasicMessage.h"


/**
 * LoginMessage class to exchange login info
 */
class LoginMessage : public BasicMessage {

private:
    ///Username
    std::string username;

    ///Password
    std::string password;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ///Classic constructor with all parameters given (Type is automatically set)
    LoginMessage(unsigned int editorId, std::string username,
                 std::string password);

    LoginMessage() = default;

    ///Return the username
    std::string &getUsername();

    ///Return the password
    std::string &getPassword();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_LOGINMESSAGE_H
