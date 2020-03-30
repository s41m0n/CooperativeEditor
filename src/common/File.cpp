#include "File.h"

File::File(QString name, FileText text) : fileName(std::move(name)),
                                          fileText(std::move(text)) {}


QString File::getFileName() {
  return this->fileName;
}

FileText &File::getFileText() {
  return fileText;
}

QDataStream &File::serialize(QDataStream &stream) const {
  stream << fileName << fileText;
  return stream;
}

QDataStream &File::deserialize(QDataStream &stream) {
  stream >> fileName >> fileText;
  return stream;
}

std::string File::toStdString(int level) {
  std::string tmp(std::string(level, '\t') + "File{\n" +
                  std::string(level + 1, '\t') + "filename: " +
                  fileName.toStdString() + "\n" +
                  std::string(level + 1, '\t') + "text: ");
  for (auto &val: fileText)
    for(auto &s: val)
    tmp += s.getChar().toLatin1();
  tmp += "\n" + std::string(level, '\t') + "}";
  return tmp;
}

