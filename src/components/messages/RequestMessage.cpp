#include "RequestMessage.h"

RequestMessage::RequestMessage(Type msgType, unsigned int editorId,
                               std::string &filename) : BasicMessage(msgType,
                                                                     editorId),
                                                        filename(filename) {
}

std::string RequestMessage::getFilename() {
  return filename;
}

RequestMessage::RequestMessage(BasicMessage &&msg) : BasicMessage(
        std::move(msg)) {
}

std::string RequestMessage::toString(int level) {
  return std::string(level, '\t') + "RequestMessage{\n" +
         std::string(level + 1, '\t') + "msgType: " +
         std::to_string(static_cast<int>(msgType)) + "\n" +
         std::string(level + 1, '\t') + "editorId: " +
         std::to_string(editorId) + "\n" +
         std::string(level + 1, '\t') + "file/s: " + filename + "\n" +
         std::string(level, '\t') + "}";
}
