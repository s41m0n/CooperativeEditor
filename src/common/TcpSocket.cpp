#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), clientID(-1) {
  connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

qint32 TcpSocket::getClientID() {
  return clientID;
}

void TcpSocket::setClientID(qintptr idToSet) {
  clientID = idToSet;
}

bool TcpSocket::isMessageAvailable() {
  if (!header.isValid() && bytesAvailable() >= header.objectSize()) {
    QDataStream ds(this);
    ds >> header;
  }
  return header.isValid() && bytesAvailable() >= header.getSize();
}

void TcpSocket::sendMsg(Header &headerToSend, QByteArray &val) {
  QDataStream ds(this);
    ds << headerToSend;
    write(val);
    if(bytesToWrite() > 1000000)
      flush();
    spdlog::debug("Sent message\n{}", headerToSend.toStdString());
}

void TcpSocket::onReadyRead() {

  spdlog::error("PORCODIOOOOOOOOOO {}", bytesAvailable());
  while (isMessageAvailable()) {
    QByteArray buf = read(header.getSize());
    spdlog::debug("Read message\n{}", header.toStdString());
    emit messageReceived(header, buf);
    header = {};
  }
}