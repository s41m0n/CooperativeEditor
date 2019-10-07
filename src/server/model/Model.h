#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <vector>
#include <map>
#include <string>

#include "components/Symbol.h"

class Model {

private:
    ///The map of the connection, containing the editorId and the pointer to the connection
    std::map<std::string, std::vector<Symbol>> openedFiles;

    ///The mutexes for the opened files
    std::map<std::string, std::unique_ptr<std::mutex>> openedFilesMutexes;

    ///The mutex for the files mutex map
    std::mutex openedFilesMapMutex;

    ///The map of the current file for each user
    std::map<unsigned int, std::string> usersFile;

    ///Editor Id unique generator
    std::atomic<unsigned int> idGenerator;

    ///Set of all available files
    std::string availableFiles;

    ///Method to write on file the respective sequence of symbols
    void storeFileSymbols(std::string &filename);

    ///Method to restore from file the respective sequence of symbols
    void loadFileSymbols(std::string &filename);

public:
    ///Classic constructor
    Model();

    ///Classic destructor
    ~Model();

    ///Method to generate a new editor id
    unsigned int generateEditorId();

    void userInsert(unsigned int connId, Symbol &symbol);

    void userErase(unsigned int connId, Symbol &symbol);

    ///Method called when a user requests to create a file
    bool createFileByUser(unsigned int connId, std::string filename);

    ///Method called when a user requests to open a file
    bool openFileByUser(unsigned int connId, std::string filename);

    ///Returns the list (string) of all available files
    std::string &getAvailableFiles();

    ///Returns the list of symbols for the file the user connId has requested
    std::vector<Symbol> &getFileSymbolList(unsigned int connId);

};


#endif //COOPERATIVEEDITOR_MODEL_H
