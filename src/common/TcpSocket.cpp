#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this), id(0) {
  connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

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

void TcpSocket::sendMsg(Header &headerToSend, QByteArray &val) {
    ds << headerToSend;
    write(val);
}

void TcpSocket::onReadyRead() {

  while (isMessageAvailable()) {
    QByteArray buf = read(header.getSize());
    emit messageReceived(header, buf);
    spdlog::debug("Read message with header {} ", header.toStdString());
    header = {};
  }
}