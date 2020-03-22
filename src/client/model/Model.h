#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QVector>
#include <QString>
#include <string>
#include <memory>
#include <src/common/File.h>

#include "common/User.h"
#include "components/Symbol.h"

/**Model class for the client
 *
 */
class Model {

private:

    unsigned editorId;

    File file;

    User user;

    unsigned digitGenerator;

    Symbol generateSymbol(int index, QChar value);

public:

    Model();

    User getUser();

    ///Crdt local method to insert a symbol
    Symbol &localInsert(int index, QChar value, bool attributes[Attribute::ATTRIBUTE_SIZE]);

    ///Crdt local method to erase a symbol
    Symbol localErase(int index);

    Symbol &localUpdate(int index, bool attributes[Attribute::ATTRIBUTE_SIZE]);

    ///Method to update the list of symbol after a remote insertion
    int remoteInsert(Symbol &symbol);

    ///Method to update the list of symbol after a remote deletion
    int remoteErase(Symbol &symbol);

    int remoteUpdate(Symbol &symbol);

    void setEditorId(unsigned newEditorId);

    unsigned getEditorId();

    ///Method to set the current opened file name
    void setCurrentFile(File &fileToSet);

    ///Method to set the current user data
    void setCurrentUser(User &userToSet);

    FileText getFileText();

    ///Return the file content
    QString textify();

    ///Return the file content as a std::string
    std::string textifyToStdString();

};


#endif //COOPERATIVEEDITOR_MODEL_H
