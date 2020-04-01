#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), ds(this), clientID(-1) {
  connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

qint32 TcpSocket::getClientID() {
  return clientID;
}

void TcpSocket::setClientID(qintptr idToSet) {
  clientID = idToSet;
}

bool TcpSocket::isMessageAvailable() {
  if (!header.isValid() && bytesAvailable() >= Header::OBJECT_SIZE) {
    ds >> header;
  }
  return header.isValid() && bytesAvailable() >= header.getSize();
}

void TcpSocket::sendMsg(Header &headerToSend, QByteArray &val) {
    ds << headerToSend;
    write(val);
    //flush()
    spdlog::debug("Sent message\n{}", headerToSend.toStdString());
}

void TcpSocket::onReadyRead() {

  while (isMessageAvailable()) {
    QByteArray buf = read(header.getSize());
    emit messageReceived(header, buf);
    spdlog::debug("Read message\n{}", header.toStdString());
    header = {};
  }
}