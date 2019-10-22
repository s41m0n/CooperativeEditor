#include "ServerFile.h"

ServerFile::ServerFile(QString name, FileText text) : File(std::move(name),
                                                           std::move(text)) {}
