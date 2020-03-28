#ifndef COOPERATIVEEDITOR_FILEMESSAGE_H
#define COOPERATIVEEDITOR_FILEMESSAGE_H

#include "BasicMessage.h"
#include "common/File.h"

/**
 * FileContentMessage class, represents a File content transfer message between
 * client-server
 *
 */
class FileMessage : public BasicMessage {

private:
  File file;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  FileMessage(unsigned editorId, File &file);

  FileMessage() = default;

  File &getFile();

  std::string toStdString(int level = 0) override;

  static FileMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_FILEMESSAGE_H
