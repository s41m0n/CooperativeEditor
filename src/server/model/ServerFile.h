#ifndef COOPERATIVEEDITOR_SERVERFILE_H
#define COOPERATIVEEDITOR_SERVERFILE_H

#include <QtCore/QString>
#include <src/components/Symbol.h>
#include <src/common/File.h>
#include <mutex>

/**
 * Specialized function to contain a file server-side
 */
class ServerFile : public File {
private:
  /// The file ID
  qint32 fileID;
public:
    explicit ServerFile(qint32 fileID, QString name, FileText text = {});

    std::mutex mutex;

    /**
     * Function to return the file identifier
     * @return the fileID
     */
    qint32 getFileID();
};

#endif //COOPERATIVEEDITOR_SERVERFILE_H
