#ifndef COOPERATIVEEDITOR_FILECONTENTMESSAGE_H
#define COOPERATIVEEDITOR_FILECONTENTMESSAGE_H

#include <QVector>

#include "BasicMessage.h"
#include "components/Symbol.h"


/**
 * FileContentMessage class, represents a File content transfer message between client-server
 *
 */
class FileContentMessage : public BasicMessage {

private:

    QVector<Symbol> symbols;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:

    FileContentMessage(unsigned editorId,
                       QVector<Symbol> &symbols);

    FileContentMessage() = default;

    QVector<Symbol> &getSymbols();
    
    std::string toStdString(int level = 0) override;

};

#endif //COOPERATIVEEDITOR_FILECONTENTMESSAGE_H
