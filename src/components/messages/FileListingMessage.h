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
  FileListingMessage(unsigned editorId, QVector<QString> files);

  FileListingMessage() = default;

  QVector<QString> &getFiles();

  std::string toStdString(int level = 0) override;

  static FileListingMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
