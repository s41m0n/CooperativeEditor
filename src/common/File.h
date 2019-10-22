#ifndef COOPERATIVEEDITOR_FILE_H
#define COOPERATIVEEDITOR_FILE_H

#include <QtCore/QString>
#include <src/components/Symbol.h>
#include <mutex>

class File {
protected:

    QString fileName;

    FileText fileText;

public:

    explicit File(QString name, FileText text = {});

    File() = default;

    QString getFileName();

    FileText &getFileText();

    friend QDataStream &operator<<(QDataStream &stream, const File &val);

    friend QDataStream &operator>>(QDataStream &stream, File &val);

};

#endif //COOPERATIVEEDITOR_FILE_H
