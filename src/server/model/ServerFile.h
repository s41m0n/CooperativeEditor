#ifndef COOPERATIVEEDITOR_SERVERFILE_H
#define COOPERATIVEEDITOR_SERVERFILE_H

#include <QtCore/QString>
#include <src/components/Symbol.h>
#include <src/common/File.h>
#include <mutex>

class ServerFile : public File {
private:
  int fileID;
public:
    explicit ServerFile(int fileID, QString name, FileText text = {});

    std::mutex mutex;

    int getFileID();
};

#endif //COOPERATIVEEDITOR_SERVERFILE_H
