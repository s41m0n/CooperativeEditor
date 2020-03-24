#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this), id(0) {}

unsigned TcpSocket::getIdentifier() {
  return id;
}

void TcpSocket::setIdentifier(unsigned idToSet) {
  id = idToSet;
}

bool TcpSocket::isMessageAvailable() {
  if (!header.isValid() && bytesAvailable() >= header.objectSize()) {
    ds >> header;
  }
  return header.isValid() && bytesAvailable() >= header.getSize();
}

void TcpSocket::sendMsg(Type type, BasicMessage &val) {

  QByteArray tmpBuffer;
  QDataStream dataStream(&tmpBuffer, QIODevice::WriteOnly);
  dataStream << val;

  Header tmpHeader(tmpBuffer.size(), type);

  ds << tmpHeader << val;
  spdlog::debug("Sent Message!\n{}\n{}", tmpHeader.toStdString(),
                val.toStdString());
}

void TcpSocket::sendMsg(Header &headerToSend, BasicMessage &val) {
    ds << headerToSend << val;
    spdlog::debug("Sent Message!\n{}\n{}", headerToSend.toStdString(),
                  val.toStdString());
}

BasicMessage *TcpSocket::readMsg() {
  BasicMessage *msg = nullptr;

  switch (header.getType()) {
    case Type::U_REGISTER_KO :
    case Type::F_FILE_KO :
    case Type::U_LOGIN_KO :
    case Type::U_DISCONNECTED :
    case Type::U_CONNECT : {
      msg = new BasicMessage();
      break;
    }
    case Type::U_LOGIN :
    case Type::U_CONNECTED :
    case Type::U_LOGIN_OK :
    case Type::U_REGISTER_OK :
    case Type::U_REGISTER : {
      msg = new UserMessage();
      break;
    }
    case Type::F_LISTING: {
      msg = new FileListingMessage();
      break;
    }
    case Type::F_CREATE:
    case Type::F_OPEN: {
      msg = new RequestMessage();
      break;
    }
    case Type::F_FILE_OK: {
      msg = new FileMessage();
      break;
    }
    case Type::S_INSERT :
    case Type::S_ERASE:
    case Type::S_UPDATE_ATTRIBUTE: {
      msg = new CrdtMessage();
      break;
    }
    case Type::UNKNOWN:
    default: {
      throw std::runtime_error(
              "Unknown type: forgot to add it to TcpSocket readMsg switch?");
    }
  }
  ds >> *msg;
  spdlog::debug("Received Message!\n{}\n{}", header.toStdString(),
                msg->toStdString());
  header = {};
  return msg;
}

Header &TcpSocket::getHeader() {
  return header;
}
