#include "screenshareserver.h"
#include <iostream>
#include <QTcpSocket>
#include <QDataStream>
#include <QImage>
#include <QPixmap>

ScreenshareServer::ScreenshareServer(QObject *parent)
    : QObject{parent}
{
    _server = new QTcpServer(this);
    connect(_server, &QTcpServer::newConnection, this, &ScreenshareServer::newConnection);
    if (_server->listen(QHostAddress::Any, 8084)) {
        std::cout << "Server started successfully!" << std::endl;
    } else {
        //
        // Signal error; send Response to server
        //
        std::cout << "Server could not start" << std::endl;
    }
}

void ScreenshareServer::newConnection()
{
    QTcpSocket* socket = _server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &ScreenshareServer::readSocket);
    sendMessage(socket, "connect", "", "", "");
}

void ScreenshareServer::readSocket()
{
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

    buffer = buffer.mid(128);

    if (instruction == "connect") {
        std::cout << instruction.toStdString() << std::endl;
        _lastFrame = new QPixmap();
    }
    else if (instruction == "frame") {
        _lastFrame->loadFromData(buffer);
        emit updateSize();
        //_lastFrame->scaled(_size.width()-100, _size.height()-100, Qt::KeepAspectRatio);
        emit setFrame(_lastFrame);
        /*if (_lastFrame != nullptr) {
            delete _lastFrame;
        }*/
    }
}

void ScreenshareServer::sendMessage(QTcpSocket* socket, QString instruction, QString arg1, QString arg2, QByteArray data) {
    //socket.connectToHost(QString::fromStdString(_hostname), _port);
    if (socket->isOpen() && socket->isValid()) {
        QDataStream socketStream(socket);
        socketStream.setVersion(QDataStream::Qt_5_12);

        QByteArray header;
        header.prepend(QString::fromStdString("instruction:" + instruction.toStdString() + ",arg1:" + arg1.toStdString() + ",args2:" + arg2.toStdString() + ";").toUtf8());
        header.resize(128);

        QByteArray byteArray = data;
        byteArray.prepend(header);

        socketStream << byteArray;
    } else {
        // QMessageBox::warning(new QWidget(), "Keine Verbindung zum Server!", "Es kann keine Verbindung zum Server hergestellt werden. Starten Sie die App neu und probieren Sie es erneut.", QMessageBox::Ok);
        exit(0);
    }
}
