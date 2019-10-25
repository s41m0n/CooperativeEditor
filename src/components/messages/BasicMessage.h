#ifndef COOPERATIVEEDITOR_BASICMESSAGE_H
#define COOPERATIVEEDITOR_BASICMESSAGE_H

#include "common/Serializable.h"

/**
 * BasicMessage class, represents a basic message between client-server
 *
 */
class BasicMessage : public Serializable {

protected:

    Type msgType;

    unsigned editorId;

public:

    BasicMessage(Type msgType, unsigned editorId);

    BasicMessage();

    quint32 getEditorId();

    Type &getMsgType();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

};

#endif //COOPERATIVEEDITOR_BASICMESSAGE_H
