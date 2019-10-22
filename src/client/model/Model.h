#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QVector>
#include <QString>
#include <string>
#include <memory>
#include <src/common/File.h>

#include "components/Symbol.h"

/**Model class for the client
 *
 */
class Model {

private:

    unsigned editorId;

    std::unique_ptr<File> clientFile;

    unsigned digitGenerator;

    Symbol generateSymbol(int index, QChar value);

public:

    Model();

    ///Crdt local method to insert a symbol
    Symbol *localInsert(int index, QChar value);

    ///Crdt local method to erase a symbol
    Symbol *localErase(int index);

    ///Method to update the list of symbol after a remote insertion
    void remoteInsert(Symbol symbol);

    ///Method to update the list of symbol after a remote deletion
    void remoteErase(Symbol symbol);

    void setEditorId(unsigned newEditorId);

    unsigned getEditorId();

    ///Method to set the current opened file name
    void setCurrentFile(const QString &filename);

    ///Method to set the current opened file content
    void setCurrentFileContent(FileText &newContent);

    ///Return the file content
    QString textify();

    ///Return the file content as a std::string
    std::string textifyToStdString();

};


#endif //COOPERATIVEEDITOR_MODEL_H
