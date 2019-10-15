#ifndef COOPERATIVEEDITOR_REQUESTMESSAGE_H
#define COOPERATIVEEDITOR_REQUESTMESSAGE_H

#include <QString>

#include "BasicMessage.h"

/**
 * RequestMessage class, represents some initial setup messages between client-server
 *
 */
class RequestMessage : public BasicMessage {

private:

    QString filename;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:

    RequestMessage(Type msgType, unsigned editorId, QString filename);

    RequestMessage() = default;

    QString &getFilename();

    std::string toStdString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_REQUESTMESSAGE_H
