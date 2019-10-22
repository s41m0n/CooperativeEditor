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
    explicit File (QString name, FileText text = {});
    QString getFileName();
    FileText& getFileText();
};

#endif //COOPERATIVEEDITOR_FILE_H
