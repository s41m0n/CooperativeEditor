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

  File file;

  User user;

  Symbol generateSymbol(int line, int index, QChar value);

  QVector<Identifier> findPosBefore(int line, int index);

  QVector<Identifier> findPosAfter(int line, int index);

  QVector<Symbol> deleteSingleLine(int fromLine, int fromIndex, int toLine,
                                   int toIndex);

  QVector<Symbol> deleteMultipleLines(int fromLine, int fromIndex, int toLine,
                                      int toIndex);

  std::pair<int, int> matricize(int index);

  int linearize(std::pair<int, int> pos);

public:
  User getUser();

  /// Crdt local method to insert a symbol
  Symbol localInsert(int index, QChar value, const QVector<bool> &attributes);

  /// Crdt local method to erase a symbol
  QVector<Symbol> localErase(int from, int offset);

  Symbol localUpdate(int index, int size, Attribute attribute,
                              bool set);

  /// Method to update the list of symbol after a remote insertion
  int remoteInsert(Symbol &symbol);

  /// Method to update the list of symbol after a remote deletion
  int remoteErase(Symbol &symbol);

  int remoteUpdate(QVector<Symbol> symbol);

  void setEditorId(unsigned newEditorId);

  unsigned getEditorId();

  /// Method to set the current opened file name
  void setCurrentFile(File &fileToSet);

  /// Method to set the current user data
  void setCurrentUser(User &userToSet);

  QString getFileName();

  FileText getFileText();

  /// Return the file content
  QString textify();
};

#endif // COOPERATIVEEDITOR_MODEL_H
