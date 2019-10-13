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

public:
    ///Classic constructor with all parameters given (Type is automatically set)
    LoginMessage(unsigned int editorId, std::string username,
                 std::string password);

    ///Method build a LoginMessage from a BasicMessage
    explicit LoginMessage(BasicMessage &&msg);

    LoginMessage() = default;

    ///Return the username
    std::string &getUsername();

    ///Return the password
    std::string &getPassword();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

    ///Operator overload '<<' for LoginMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const LoginMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << val.username.c_str()
             << val.password.c_str();
      return stream;
    }

    ///Operator overload '>>' for LoginMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, LoginMessage &val) {
      char *tmp;
      stream >> tmp;
      if (tmp) {
        val.username = std::string(tmp);
      }
      stream >> tmp;
      if (tmp) {
        val.password = std::string(tmp);
      }
      delete[] tmp;
      return stream;
    };
};


#endif //COOPERATIVEEDITOR_LOGINMESSAGE_H
