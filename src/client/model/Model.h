#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QVector>
#include <QString>
#include <string>
#include <memory>
#include <src/common/File.h>
#include <memory>

#include "src/include/lib/crdt/CrdtAlgorithm.h"
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
    localInsert(int index, QString value, const QVector<bool> &attributes);

    ///Crdt local method to erase a symbol
    QVector<Symbol> localErase(int index, int size);

    QVector<Symbol>
    localUpdate(int index, int size, Attribute attribute, bool set);

    ///Method to update the list of symbol after a remote insertion
    QVector<int> remoteInsert(QVector<Symbol>& symbols);

    ///Method to update the list of symbol after a remote deletion
    QVector<int> remoteErase(QVector<Symbol>& symbols);

    QVector<int> remoteUpdate(QVector<Symbol>& symbols);

    void setEditorId(unsigned newEditorId);

    unsigned getEditorId();

    ///Method to set the current opened file name
    void setCurrentFile(File &fileToSet);

    ///Method to set the current user data
    void setCurrentUser(User &userToSet);

    File getFile();

    FileText &getFileText();

    ///Return the file content
    std::string toStdString();

};


#endif //COOPERATIVEEDITOR_MODEL_H
