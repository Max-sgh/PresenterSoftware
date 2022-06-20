#include "client.h"
#include <QDataStream>

Client::Client(QObject *parent, QTcpSocket* socket) : QObject(parent)
{
    _socket = socket;
    //sendMessage("getInformation", "", "", "");
    _isAvaiable = true;
}

void Client::sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data) {
    QDataStream socketStream(_socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("instruction:" + instruction + ",arg1:" + arg1 + ",arg2:" + arg2 + ";").toUtf8());
    header.resize(128);

    QByteArray byteArray = data;
    byteArray.prepend(header);

    socketStream << byteArray;
}

void Client::evaluateClientAction(QString action, QString arg2, QByteArray data)
{

}
