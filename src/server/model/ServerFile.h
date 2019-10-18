#ifndef COOPERATIVEEDITOR_SERVERFILE_H
#define COOPERATIVEEDITOR_SERVERFILE_H

#include <QtCore/QString>
#include <src/components/Symbol.h>
#include <src/common/File.h>

class ServerFile : public File {
public:
    explicit ServerFile(QString name, FileText text = {});
};

#endif //COOPERATIVEEDITOR_SERVERFILE_H
