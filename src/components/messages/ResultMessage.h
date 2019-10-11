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
    bool result;

public:
    ///Constructor with also the type (it could index different msg)
    ResultMessage(Type msgType, unsigned editorId, bool result);

    ///Movement constructor to build one from BasicMessage
    explicit ResultMessage(BasicMessage &&msg);

    ResultMessage() = default;

    ///Method to return the answer
    bool isPositive();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

    ///Operator overload '<<' for ResultMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const ResultMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << val.result;
      return stream;
    }

    ///Operator overload '>>' for ResultMessage when using QDataStream for serialization
    friend QDataStream &operator>>(QDataStream &stream, ResultMessage &val) {
      stream >> val.result;
      return stream;
    };

};

#endif //COOPERATIVEEDITOR_RESULTMESSAGE_H
