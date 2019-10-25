#ifndef COOPERATIVEEDITOR_FILEMESSAGE_H
#define COOPERATIVEEDITOR_FILEMESSAGE_H

#include <QVector>

#include "BasicMessage.h"
#include "common/File.h"


/**
 * FileContentMessage class, represents a File content transfer message between client-server
 *
 */
class FileMessage : public BasicMessage {

private:

    File file;

public:

    FileMessage(unsigned editorId,
                File &file);

    FileMessage() = default;

    File &getFile();

    std::string toStdString(int level = 0) override;

    QDataStream &serialize(QDataStream &stream) const override;

    QDataStream &deserialize(QDataStream &stream) override;

};

#endif //COOPERATIVEEDITOR_FILEMESSAGE_H
