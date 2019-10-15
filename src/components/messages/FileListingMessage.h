#ifndef COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
#define COOPERATIVEEDITOR_FILELISTINGMESSAGE_H

#include <QVector>
#include <QString>

#include "BasicMessage.h"

/**
 * FileListingMessage class, represents the list of files
 *
 */
class FileListingMessage : public BasicMessage {

private:

    QVector<QString> files;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:

    FileListingMessage(unsigned editorId,
                       QVector<QString> &files);

    FileListingMessage() = default;

    QVector<QString> &getFiles();

    std::string toStdString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
