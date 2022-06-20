#include "room.h"
#include <QDataStream>
#include <iostream>
#include <QFile>

Room::Room()
{
    char caracters[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    size_t maxindex = sizeof(caracters) - 1;
    std::string s;
    _currentSlide = -1;
    for (int i = 0; i < _IDLength; i++) {
        s += (caracters[rand() % maxindex - 1]);
    }
    _roomID = s;
}

Room::~Room() {
    delete _clientSocket;
    delete _response;
}

void Room::clientConnected() {
    // emit clientConnected();
    QDataStream socketStream(_response);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("instruction:setClient,success:success,arg1:null,arg2:null;").toUtf8());
    header.resize(128);

    QByteArray byteArray = "data";
    byteArray.prepend(header);

    socketStream << byteArray;
}

void Room::test(QByteArray b) {
    QDataStream socketStream(_response);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("instruction:Slide,success:success,arg1:1,arg2:null;").toUtf8());
    header.resize(128);

    QByteArray byteArray = b;
    byteArray.prepend(header);

    socketStream << byteArray;
}

bool Room::sendSlide(QString filename, int number) {
    QFile img(filename);
    if (img.open(QIODevice::ReadOnly)) {
        QByteArray b = img.readAll();
        sendMessage("Slide", QString::number(number), "", b);
        img.close();
        return true;
    }
    else {
        return false;
        std::cout << "Cannot open file" << filename.toStdString() << std::endl;
    }
}

std::string Room::getID() {
    return _roomID;
}

void Room::sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data) {
    QDataStream socketStream(_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("instruction:" + instruction + ",arg1:" + arg1 + ",arg2:" + arg2 + ";").toUtf8());
    header.resize(128);

    QByteArray byteArray = data;
    byteArray.prepend(header);

    socketStream << byteArray;
}

void Room::readSocket() {
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QByteArray buffer;
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    socketStream.startTransaction();
    socketStream >> buffer;

    if (!socketStream.commitTransaction()) {
        // std::cout << "waiting for Data!" << std::endl;
        return;
    }

    QString header = buffer.mid(0,128);
    QString instruction = header.split(",")[0].split(":")[1];
    QString arg1 = header.split(",")[1].split(":")[1];
    QString arg2 = header.split(",")[2].split(":")[1].split(";")[0];
    // std::cout << header.toStdString() << " " << instruction.toStdString() << " " << arg1.toStdString() << " " << arg2.toStdString() << std::endl;

    buffer = buffer.mid(128);
    if (QString::fromStdString(buffer.toStdString()) == "connected") {
        std::cout << "Connected, Hurray" << std::endl;
        //sendMessage("setInformation", QString::fromStdString(_roomID), _clientName, QByteArray::fromStdString(""));
        clientConnected();
        _isConnected = true;
    }
    if (instruction == "setInformation") {
        // std::cout << "Sended Information to the Client successfully" << std::endl;
        emit testImage();
    }
    if (instruction == "close") {
        std::cout << "close" << std::endl;
        emit closeRoom();
    }
}
