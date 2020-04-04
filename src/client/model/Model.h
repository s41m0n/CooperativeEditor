#ifndef COOPERATIVEEDITOR_MODEL_H
#define COOPERATIVEEDITOR_MODEL_H

#include <QString>
#include <QVector>
#include <memory>
#include <src/common/File.h>
#include <string>

#include "common/User.h"
#include "components/Symbol.h"
#include "src/include/lib/crdt/CrdtAlgorithm.h"

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

  /// Crdt local method to insert a symbol
  Symbol localInsert(int index, QChar value, QTextCharFormat &format);

  /// Crdt local method to erase a symbol
  Symbol localErase(int index);

  /// Method to update the list of symbol after a remote insertion
  int remoteInsert(Symbol &symbols);

  /// Method to update the list of symbol after a remote deletion
  int remoteErase(Symbol &symbols);

  void setEditorId(unsigned newEditorId);

  unsigned getEditorId();

  /// Method to set the current opened file name
  void setCurrentFile(File &fileToSet);

  /// Method to set the current user data
  void setCurrentUser(User &userToSet);

  File &getFile();

  FileText &getFileText();

  /// Return the file content
  std::string toStdString();
};

#endif // COOPERATIVEEDITOR_MODEL_H
