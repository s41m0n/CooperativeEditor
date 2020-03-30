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
  unsigned editorId{};

  FileText file;

  User user;

  unsigned digitGenerator{};

  Symbol generateSymbol(int line, int index, QChar value);

  QVector<Identifier> findPosBefore(int line, int index);

  QVector<Identifier> findPosAfter(int line, int index);

  QVector<Symbol> deleteSingleLine(int fromLine, int fromIndex, int toLine,
                                   int toIndex);

  QVector<Symbol> deleteMultipleLines(int fromLine, int fromIndex, int toLine,
                                      int toIndex);

public:
  User getUser();

  /// Crdt local method to insert a symbol
  Symbol localInsert(int line, int index, QChar value,
                              const QVector<bool> &attributes);

  /// Crdt local method to erase a symbol
  QVector<Symbol> localErase(int fromLine, int fromIndex, int toLine,
                             int toIndex);

  QVector<Symbol> localUpdate(int index, int size, Attribute attribute,
                              bool set);

  /// Method to update the list of symbol after a remote insertion
  void remoteInsert(Symbol &symbol);

  /// Method to update the list of symbol after a remote deletion
  void remoteErase(Symbol &symbol);

  int remoteUpdate(QVector<Symbol> symbol);

  void setEditorId(unsigned newEditorId);

  unsigned getEditorId();

  /// Method to set the current opened file name
  void setCurrentFile(File &fileToSet);

  /// Method to set the current user data
  void setCurrentUser(User &userToSet);

  File getFile();

  FileText getFileText();

  /// Return the file content
  QString textify();

  /// Return the file content as a std::string
  std::string textifyToStdString();
};

#endif // COOPERATIVEEDITOR_MODEL_H
