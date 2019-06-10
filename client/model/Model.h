//
// Created by s41m0n on 10/06/19.
//

#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <string>
#include <vector>
#include "../../components/Symbol.h"

class Model {

private:

    ///The specific editor ID
    int editorId;

    ///The vector of symbol, which composes the text
    std::vector<Symbol> symbols;

    ///The actual file used
    std::string currentFile;

    ///The unique digit generator
    unsigned int digitGenerator;

    ///Method to generate a symbol
    const Symbol generateSymbol(int index, char value);

    ///Method to perform the tostring on file
    void writeOnFile();

public:
    Model();
    ~Model();

    ///Crdt local method to insert a symbol
    const Symbol* localInsert(int index, char value);

    ///Crdt local method to erase a symbol
    const Symbol* localErase(int index);

    void remoteInsert(Symbol symbol);

    void remoteErase(Symbol symbol);

    ///Method to return the editorId
    const int getEditorId();

    ///Method to set the editorId once received by the server
    void setEditorId(int newEditorId);

    ///Method to set the current opened file name
    void setCurrentFile(std::string& filename);

    void setCurrentFileContent(std::vector<Symbol> newContent);

    ///Method to return the sequence of characters
    const std::string to_string();

};


#endif //COOPERATIVEEDITOR_MODEL_H
