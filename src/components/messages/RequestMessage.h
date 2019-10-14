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

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ///Classic constructor with all parameters given
    RequestMessage(Type msgType, unsigned int editorId, std::string &filename);

    RequestMessage() = default;

    ///Return the requested filename
    std::string getFilename();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_REQUESTMESSAGE_H
