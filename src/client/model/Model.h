//
// Created by s41m0n on 10/06/19.
//

#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <string>
#include <vector>

#include "components/Symbol.h"

/**Model class for the client
 *
 * @author Simone Magnani - s41m0n
 */
class Model {

private:

    ///The specific editor ID
    unsigned int editorId;

    ///The vector of symbol, which composes the text
    std::vector<Symbol> symbols;

    ///The actual file used
    std::string currentFile;

    ///The unique digit generator
    unsigned int digitGenerator;

    ///Method to generate a symbol
    Symbol generateSymbol(int index, char value);

public:
    ///Constructor
    Model();

    //Destructor
    ~Model();

    ///Crdt local method to insert a symbol
    Symbol* localInsert(int index, char value);

    ///Crdt local method to erase a symbol
    Symbol* localErase(int index);

    ///Method to update the list of symbol after a remote insertion
    void remoteInsert(Symbol symbol);

    ///Method to update the list of symbol after a remote deletion
    void remoteErase(Symbol symbol);

    ///Method to return the editorId
    unsigned int getEditorId();

    ///Method to set the editorId once received by the server
    void setEditorId(unsigned int newEditorId);

    ///Method to set the current opened file name
    void setCurrentFile(std::string& filename);

    void setCurrentFileContent(std::vector<Symbol> newContent);

    ///Method to return the sequence of characters
    std::string textify();

};


#endif //COOPERATIVEEDITOR_MODEL_H
