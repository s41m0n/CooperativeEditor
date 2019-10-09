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

public:
    ///Classic constructor with all parameters (Type is automatically set)
    FileListingMessage(unsigned int editorId,
                       std::vector<std::string> &files);

    ///Method build a FileContentMessage from a BasicMessage
    explicit FileListingMessage(BasicMessage &&msg);

    ///Return all the symbols
    std::vector<std::string> &getFiles();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

    ///Operator overload '<<' for FileListingMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const FileListingMessage &val) {
      stream << static_cast<const BasicMessage &>(val) << static_cast<quint32>(val.files.size());
      for (const auto &v : val.files) {
        stream << v.c_str();
      }
      return stream;
    }

    ///Operator overload '>>' for FileListingMessage when using QDataStream for serialization
    friend QDataStream &
    operator>>(QDataStream &stream, FileListingMessage &val) {
      quint32 length = 0;
      stream >> length;
      for (quint32 i = 0; i < length; ++i) {
        char *tmp;
        stream >> tmp;
        if (tmp)
          val.files.emplace_back(tmp);
        delete[] tmp;
      }
      return stream;
    };

};


#endif //COOPERATIVEEDITOR_FILELISTINGMESSAGE_H
