#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QVector>
#include <QString>
#include <map>
#include <mutex>
#include <memory>
#include <atomic>

#include "components/Symbol.h"

/**
 * Model server class
 */
class Model {

private:
    ///The map of the connection, containing the editorId and the pointer to the connection
    std::map<QString, QVector<Symbol>> openedFiles;

    ///The mutexes for the opened files
    std::map<QString, std::unique_ptr<std::mutex>> openedFilesMutexes;

    ///The mutex for the files mutex map
    std::mutex openedFilesMapMutex;

    ///The map of the current file for each user
    std::map<unsigned, QString> usersFile;

    ///Editor Id unique generator
    std::atomic<unsigned> idGenerator;

    ///Set of all available files
    QVector<QString> availableFiles;

    ///Method to write on file the respective sequence of symbols
    void storeFileSymbols(QString &filename);

    ///Method to restore from file the respective sequence of symbols
    void loadFileSymbols(QString &filename);

public:
    ///Classic constructor
    Model();

    ///Method to generate a new editor id
    unsigned generateEditorId();

    ///Method to handle user remote insertion
    void userInsert(unsigned connId, Symbol &symbol);

    ///Method to handle user remote deletion
    void userErase(unsigned connId, Symbol &symbol);

    ///Method called when a user requests to create a file
    bool createFileByUser(unsigned connId, const QString &filename);

    ///Method called when a user requests to open a file
    bool openFileByUser(unsigned connId, QString filename);

    ///Returns the list (string) of all available files
    QVector<QString> &getAvailableFiles();

    ///Returns the list of symbols for the file the user connId has requested
    QVector<Symbol> &getFileSymbolList(unsigned connId);

};


#endif //COOPERATIVEEDITOR_MODEL_H
