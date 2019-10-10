#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this),
                                        userAuthN(false) {}

bool TcpSocket::isUserAuthN() {
  return userAuthN;
}

void TcpSocket::setUserAuthN(bool value) {
  userAuthN = value;
}
