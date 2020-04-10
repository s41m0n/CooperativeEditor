#ifndef COOPERATIVEEDITOR_FILE_H
#define COOPERATIVEEDITOR_FILE_H

#include <QString>
#include <mutex>

#include "src/include/lib/serialization/Serializable.h"
#include <src/components/Symbol.h>

/**
 * File class, to represent a file
 */
class File : public Serializable {

protected:
  QString fileName;

  FileText fileText;

public:
  explicit File(QString name, FileText text = {});

  File() = default;

  std::string toStdString(int level) override;

  QDataStream &serialize(QDataStream &stream) const override;

  QDataStream &deserialize(QDataStream &stream) override;

  bool operator==(const QString &name) { return this->fileName == name; }

  QString &getFileName();

  FileText &getFileText();
};

#endif // COOPERATIVEEDITOR_FILE_H
