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

void TcpSocket::sendMsg(BasicMessage &val) {
  QByteArray byteArray;
  QDataStream dataStream(&byteArray, QIODevice::WriteOnly);
  dataStream << val;
  Header tmp(byteArray.size(), val.getMsgType());

  ds << tmp << byteArray;
  spdlog::debug("Sent Message! {}\n{}", byteArray.size(), val.toStdString());
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
  spdlog::debug("Received Message!{}\n{}",header.objectSize() + msg->objectSize(), msg->toStdString());
  header = {};
  return msg;
}
