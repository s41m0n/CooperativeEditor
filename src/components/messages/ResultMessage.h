#ifndef COOPERATIVEEDITOR_RESULTMESSAGE_H
#define COOPERATIVEEDITOR_RESULTMESSAGE_H

#include "BasicMessage.h"

/**
 * ResultMessage class, represents a result of a specific request (LOGIN, FILE)
 *
 */
class ResultMessage : public BasicMessage {

private:

    bool result{};

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ResultMessage(Type msgType, unsigned editorId, bool result);

    ResultMessage() = default;
    
    bool isPositive();

    std::string toStdString(int level = 0) override;

};

#endif //COOPERATIVEEDITOR_RESULTMESSAGE_H
