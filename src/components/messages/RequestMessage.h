#ifndef COOPERATIVEEDITOR_REQUESTMESSAGE_H
#define COOPERATIVEEDITOR_REQUESTMESSAGE_H

#include "BasicMessage.h"

/**
 * RequestMessage class, represents some initial setup messages between client-server
 *
 */
class RequestMessage : public BasicMessage {

private:
    ///The name of the requested file
    std::string filename;

public:
    ///Classic constructor with all parameters given
    RequestMessage(Type msgType, unsigned int editorId, std::string &filename);

    ///Method build a RequestMessage from a BasicMessage
    explicit RequestMessage(BasicMessage &&msg);

    ///Return the requested filename
    std::string getFilename();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

    ///Operator overload '<<' for RequestMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const RequestMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << val.filename.c_str();
      return stream;
    }

    ///Operator overload '>>' for RequestMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, RequestMessage &val) {
      char *tmp;
      stream >> tmp;
      if (tmp)
        val.filename = std::string(tmp);
      delete[] tmp;
      return stream;
    };
};


#endif //COOPERATIVEEDITOR_REQUESTMESSAGE_H
