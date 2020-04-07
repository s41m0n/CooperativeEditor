#include "ServerFile.h"

ServerFile::ServerFile(int fileID, QString name, FileText text)
    : File(std::move(name), std::move(text)), fileID(fileID) {}

int ServerFile::getFileID() { return fileID; }