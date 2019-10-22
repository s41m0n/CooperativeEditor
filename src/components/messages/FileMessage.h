#ifndef COOPERATIVEEDITOR_FILEMESSAGE_H
#define COOPERATIVEEDITOR_FILEMESSAGE_H

#include <QVector>

#include "BasicMessage.h"
#include "components/Symbol.h"
#include "common/File.h"


/**
 * FileContentMessage class, represents a File content transfer message between client-server
 *
 */
class FileMessage : public BasicMessage {

private:

    File file;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:

    FileMessage(unsigned editorId,
                File &file);

    FileMessage() = default;

    File &getFile();

    std::string toStdString(int level = 0) override;

};

#endif //COOPERATIVEEDITOR_FILEMESSAGE_H
