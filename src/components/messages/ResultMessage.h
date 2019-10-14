#ifndef COOPERATIVEEDITOR_RESULTMESSAGE_H
#define COOPERATIVEEDITOR_RESULTMESSAGE_H

#include "BasicMessage.h"

/**
 * ResultMessage class, represents a result of a specific request (LOGIN, FILE)
 *
 */
class ResultMessage : public BasicMessage {

private:
    ///The OK/KO answer
    bool result{};

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ///Constructor with also the type (it could index different msg)
    ResultMessage(Type msgType, unsigned editorId, bool result);

    ResultMessage() = default;

    ///Method to return the answer
    bool isPositive();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

};

#endif //COOPERATIVEEDITOR_RESULTMESSAGE_H
