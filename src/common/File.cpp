#include "File.h"

File::File(QString name, FileText text) : fileName(name), fileText(text) {}

QString File::getFileName() {
    return this->fileName;
}

FileText& File::getFileText() {
    return fileText;
}