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

/**
 * Model class for the client
 */
class Model {

private:
  quint32 editorId;

  File file;

  User user;

  /**
   * Function to generate a symbol
   * @param index  the index of the symbol to be generated
   * @param value  the character
   * @return the generated symbol
   */
  Symbol generateSymbol(int index, QChar value);

public:
  Model();

  /**
   * Method to handle a local insert
   * @param index  the index of the insertion
   * @param value  the character inserted
   * @param format  the current format
   * @return the symbol inserted
   */
  Symbol localInsert(int index, QChar value, QTextCharFormat &format);

  /**
   * Function to handle a local deletion
   * @param index the index of the deletion
   * @return  the Symbol erased
   */
  Symbol localErase(int index);

  /**
   * Function to handle a remote insertion
   * @param symbol inserted
   * @return the index of the symbol
   */
  int remoteInsert(Symbol &symbols);

  /**
   * Function to handle a remote deletion
   * @param symbol erased
   * @return the index of the symbol
   */
  int remoteErase(Symbol &symbol);

  /**
   * Function to set the current editorID
   * @param newEditorId
   */
  void setEditorId(quint32 newEditorId);

  /**
   * Function to get the current editor ID
   * @return  the current editor id
   */
  quint32 getEditorId();

  /**
   * Function to set the current file handled
   * @param fileToSet the file to be set
   */
  void setCurrentFile(File &fileToSet);

  /**
   * Function to set the current user
   * @param userToSet
   */
  void setCurrentUser(User &userToSet);

  /**
   * Function to get the current user
   * @return the current user
   */
  User &getUser();

  /**
   * Function to get the current file
   * @return  the current file
   */
  File &getFile();

  std::string toStdString();
};

#endif // COOPERATIVEEDITOR_MODEL_H
