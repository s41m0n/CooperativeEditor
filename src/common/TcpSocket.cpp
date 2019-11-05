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
    case Type::REGISTER_KO :
    case Type::FILE_KO :
    case Type::LOGIN_KO :
    case Type::CONNECT : {
      msg = new BasicMessage();
      break;
    }
    case Type::LOGIN :
    case Type::LOGIN_OK :
    case Type::REGISTER_OK :
    case Type::REGISTER : {
      msg = new UserMessage();
      break;
    }
    case Type::LISTING: {
      msg = new FileListingMessage();
      break;
    }
    case Type::CREATE: {
      msg = new RequestMessage();
      break;
    }
    case Type::OPEN: {
      msg = new RequestMessage();
      break;
    }
    case Type::FILE_OK: {
      msg = new FileMessage();
      break;
    }
    case Type::INSERT :
    case Type::ERASE: {
      msg = new CrdtMessage();
      break;
    }
    case Type::UNKNOWN: {
      return msg;
    }
    default: {
      return msg;
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
