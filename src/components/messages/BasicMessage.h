#ifndef COOPERATIVEEDITOR_BASICMESSAGE_H
#define COOPERATIVEEDITOR_BASICMESSAGE_H

#include "src/include/lib/serialization/Serializable.h"

/**
 * BasicMessage class, represents a basic message between client-server
 *
 */
class BasicMessage : public Serializable {

protected:

    unsigned editorId;

public:

    BasicMessage(unsigned editorId);

    BasicMessage();

    quint32 getEditorId();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

};

#endif //COOPERATIVEEDITOR_BASICMESSAGE_H
