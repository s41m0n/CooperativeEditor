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
    QVector<Symbol>
    localInsert(int index, QString value, const QVector<bool>& attributes);

    ///Crdt local method to erase a symbol
    QVector<Symbol> localErase(int index, int size);

    QVector<Symbol> localUpdate(int index, int size, const QVector<bool>& attributes);

    ///Method to update the list of symbol after a remote insertion
    int remoteInsert(QVector<Symbol> symbol);

    ///Method to update the list of symbol after a remote deletion
    int remoteErase(QVector<Symbol> symbol);

    int remoteUpdate(QVector<Symbol> symbol);

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
