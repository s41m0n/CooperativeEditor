#include "File.h"

#include <utility>

File::File(QString name, FileText text) : fileName(std::move(name)),
                                          fileText(std::move(text)) {}


QString File::getFileName() {
  return this->fileName;
}

FileText &File::getFileText() {
  return fileText;
}

QDataStream &operator<<(QDataStream &stream, const File &val) {
  stream << val.fileName << val.fileText;
  return stream;
}

QDataStream &operator>>(QDataStream &stream, File &val) {
  stream >> val.fileName >> val.fileText;
  return stream;
}
