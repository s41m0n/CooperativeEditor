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
  FileMessage(quint32 editorId, File &file);

  FileMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the file in this message
   * @return  the file carried
   */
  File &getFile();

  /**
   * Function to create a FileMessage from raw bytes
   * @param buf  the raw bytes
   * @return the parsed message
   */
  static FileMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_FILEMESSAGE_H
