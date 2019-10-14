//
// Created by s41m0n on 10/10/19.
//

#ifndef COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
#define COOPERATIVEEDITOR_FILELISTINGMESSAGE_H

#include "BasicMessage.h"

/**
 * FileListingMessage class, represents the list of files
 *
 */
class FileListingMessage : public BasicMessage {

private:
    ///Vector of symbols for the previous established files
    std::vector<std::string> files;

    void serialize(QDataStream &stream) override;

    void deserialize(QDataStream &stream) override;

public:
    ///Classic constructor with all parameters (Type is automatically set)
    FileListingMessage(unsigned int editorId,
                       std::vector<std::string> &files);

    FileListingMessage() = default;

    ///Return all the symbols
    std::vector<std::string> &getFiles();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

};


#endif //COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
