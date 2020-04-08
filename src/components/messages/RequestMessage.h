#ifndef COOPERATIVEEDITOR_REQUESTMESSAGE_H
#define COOPERATIVEEDITOR_REQUESTMESSAGE_H

#include <QString>

#include "BasicMessage.h"

/**
 * RequestMessage class, represents some initial setup messages between
 * client-server
 *
 */
class RequestMessage : public BasicMessage {

private:
  QString resource;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

public:
  RequestMessage(quint32 editorId, QString filename);

  RequestMessage() = default;

  std::string toStdString(int level = 0) override;

  /**
   * Function to get the resource name
   * @return the resource name
   */
  QString &getResource();

  /**
   * Function to create a RequestMessage from raw bytes
   * @param buf  the raw bytes
   * @return the parsed message
   */
  static RequestMessage fromQByteArray(QByteArray &buf);
};

#endif // COOPERATIVEEDITOR_REQUESTMESSAGE_H
