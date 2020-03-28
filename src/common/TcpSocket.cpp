#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this){
  connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

bool TcpSocket::isMessageAvailable() {
  if (!header.isValid() && bytesAvailable() >= header.objectSize()) {
    ds >> header;
  }
  return header.isValid() && bytesAvailable() >= header.getSize();
}

void TcpSocket::sendMsg(Header &headerToSend, QByteArray &val) {
    ds << headerToSend;
    write(val);
}

void TcpSocket::onReadyRead() {

  while (isMessageAvailable()) {
    QByteArray buf = read(header.getSize());
    emit messageReceived(header, buf);
    header = {};
  }
}