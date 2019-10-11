#ifndef COOPERATIVEEDITOR_FILECONTENTMESSAGE_H
#define COOPERATIVEEDITOR_FILECONTENTMESSAGE_H

#include "BasicMessage.h"
#include "components/Symbol.h"

/**
 * FileContentMessage class, represents a File content transfer message between client-server
 *
 */
class FileContentMessage : public BasicMessage {

private:
    ///Vector of symbols for the previous established files
    std::vector<Symbol> symbols;

public:
    ///Classic constructor with all parameters (Type is automatically set)
    FileContentMessage(unsigned int editorId,
                       std::vector<Symbol> &symbols);

    ///Method build a FileContentMessage from a BasicMessage
    explicit FileContentMessage(BasicMessage &&msg);

    FileContentMessage() = default;

    ///Return all the symbols
    std::vector<Symbol> &getSymbols();

    ///Method to print in human-readable format the message with indent
    std::string toString(int level = 0) override;

    ///Operator overload '<<' for FileContentMessage when using QDataStream for serialization
    friend QDataStream &
    operator<<(QDataStream &stream, const FileContentMessage &val) {
      stream << static_cast<const BasicMessage &>(val)
             << static_cast<quint32>(val.symbols.size());
      for (auto &tmp: val.symbols)
        stream << tmp;
      return stream;
    }

    ///Operator overload '>>' for FileContentMessage when using QDataStream for serialization
    friend QDataStream &
    operator>>(QDataStream &stream, FileContentMessage &val) {
      quint32 size;
      stream >> size;
      Symbol tmp;
      for (quint32 i = 0; i < size; i++) {
        stream >> tmp;
        val.symbols.emplace_back(tmp);
      }
      return stream;
    };

};

#endif //COOPERATIVEEDITOR_FILECONTENTMESSAGE_H
