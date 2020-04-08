#ifndef COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
#define COOPERATIVEEDITOR_FILELISTINGMESSAGE_H

#include <QString>
#include <QVector>

#include "BasicMessage.h"

/**
 * FileListingMessage class, represents the list of files
 *
 */
class FileListingMessage : public BasicMessage {

private:
  QVector<QString> files;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  FileListingMessage(quint32 editorId, QVector<QString> files);

  FileListingMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the list of files
   * @return the list of files
   */
  QVector<QString> &getFiles();

  /**
   * Function to create a FileListingMessage from raw bytes
   * @param buf  the raw bytes
   * @return the parsed message
   */
  static FileListingMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
