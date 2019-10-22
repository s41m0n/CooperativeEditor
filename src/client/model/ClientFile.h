#ifndef COOPERATIVEEDITOR_CLIENTFILE_H
#define COOPERATIVEEDITOR_CLIENTFILE_H

#include <QtCore/QString>
#include <src/components/Symbol.h>
#include <src/common/File.h>

class ClientFile : public File {
public:
    ClientFile(QString name, FileText text = {});
};

#endif //COOPERATIVEEDITOR_CLIENTFILE_H
