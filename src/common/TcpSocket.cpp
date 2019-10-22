#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this),
                                        userAuthN(false), id(0) {}

unsigned TcpSocket::getIdentifier() {
  return id;
}

void TcpSocket::setIdentifier(unsigned idToSet) {
  id = idToSet;
}
