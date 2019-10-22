#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QVector>
#include <QString>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>

#include "components/Symbol.h"
#include "ServerFile.h"

/**
 * Model server class
 */
class Model {

private:

    ///Set of all available files
    QVector<QString> availableFiles;

    ///The map of the current file for each user
    std::map<unsigned, std::shared_ptr<ServerFile>> usersFile;

    ///The mutex for the usersFile map
    std::mutex usersFileMutex;

    ///Editor Id unique generator
    std::atomic<unsigned> idGenerator;

    ///Method to write on file the respective sequence of symbols
    static void storeFileSymbols(const std::shared_ptr<ServerFile> &serverFile);

    ///Method to restore from file the respective sequence of symbols
    static void loadFileSymbols(const std::shared_ptr<ServerFile> &serverFile);

public:
    ///Classic constructor
    Model();

    ///Method to handle user remote insertion
    void userInsert(unsigned connId, Symbol &symbol);

    ///Method to handle user remote deletion
    void userErase(unsigned connId, Symbol &symbol);

    void removeConnection(unsigned connId);

    ///Method called when a user requests to create a file
    bool createFileByUser(unsigned connId, const QString &filename);

    ///Method called when a user requests to open a file
    bool openFileByUser(unsigned connId, QString filename);

    ///Returns the list (string) of all available files
    QVector<QString> &getAvailableFiles();

    ///Returns the list of symbols for the file the user connId has requested
    FileText &getFileSymbolList(unsigned connId);

};


#endif //COOPERATIVEEDITOR_MODEL_H
