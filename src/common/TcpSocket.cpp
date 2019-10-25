#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this), id(0) {}

unsigned TcpSocket::getIdentifier() {
  return id;
}

void TcpSocket::setIdentifier(unsigned idToSet) {
  id = idToSet;
}

bool TcpSocket::isMessageAvailable() {
  if (!header.isValid() && bytesAvailable() >= sizeof(Header)) {
    ds >> header;
  }
  return header.isValid() && bytesAvailable() >= header.getSize();
}
