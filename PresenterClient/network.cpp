#include "network.h"
#include "strokerenderer.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <iostream>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QFile>
#include <QHostInfo>
#include <QNetworkInterface>
#include "screenshareserver.h"

Network::Network(QObject *parent) : QObject(parent)
{
    /*_server = new QTcpServer();
    if(!_server->listen(QHostAddress::Any, 8081))
    {
        std::cout << "Server could not start" << std::endl;
    }
    else
    {
        std::cout << "Server started" << std::endl;
        connect(this->_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }*/
    _socket = new QTcpSocket();
    _socket->connectToHost("192.168.178.20", 8083);
    if (_socket->waitForConnected(3000)) {
        connect(_socket, &QTcpSocket::readyRead, this, &Network::readSocket);
        sendMessage("registerClient", this->name, this->IP, "");
    } else {
        std::cout << "Can´t connect to Server!" << std::endl;
        exit(1);
    }
}

void Network::newConnection() {
    QTcpSocket *socket = _server->nextPendingConnection();
    QMessageBox msg;
    msg.setText("Verbindung annehmen?");
    msg.setInformativeText("Wollen Sie die Verbindung mit " + socket->peerAddress().toString() + " akzeptieren?");
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int ret = msg.exec();
    if (ret == QMessageBox::Yes) {
        std::cout << "Yes" << std::endl;
        connect(socket, &QTcpSocket::readyRead, this, &Network::readSocket);
        // sendMessage(socket, "connected");
    } else {
        std::cout << "No" << std::endl;
        // sendMessage(socket, "access denied");
    }
    connect(socket, &QTcpSocket::readyRead, this, &Network::readSocket);
}

void Network::readSocket() {
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

    // DEPRECATED
    /*if (instruction == "coect") {
        std::cout << "Connect" << std::endl;
    }*/
    if (instruction == "newSlide") {
        /*bool inCache = false;
        int num;
        int slideNum = std::atoi(arg1.toStdString().c_str());
        for (int i = 0; i < (int)_slideNums.size(); i++) {
            if (_slideNums[i] == slideNum) {
                inCache = true;
                num = i;
                break;
            }
        }*/
        /*if (inCache) {
            setSlide(_slides[num]);
            sendResponse(socket, instruction, "false", "", QByteArray::fromStdString("null"));
            return;
        } else {
            sendResponse(socket, instruction, "true", "", QByteArray::fromStdString("null"));
        }*/
        std::cout << instruction.toStdString() << std::endl;
        QPixmap* pix = new QPixmap();
        pix->loadFromData(buffer);
        pix->save("test.jpg");
        emit setSlide(pix);
    }
    // DEPRECATED
    /*else if (instruction == "setInformation") {
        _roomID = arg1;
        _name = arg2;
        sendResponse(socket, instruction, "success", "success", QByteArray::fromStdString(""));
        emit makePresentation();
    }*/
    else if (instruction == "Slide") {
        std::cout << instruction.toStdString() << " " << buffer.size() << std::endl;
        QPixmap* pix = new QPixmap();
        pix->loadFromData(buffer);
        _currentSlide = pix;
        _origImage = pix;
        emit setSlide(pix);

        //pix->save("test2.jpg");
    }
    else if (instruction == "close") {
        emit closeRoom();
        _slides.clear();
        _name = "";
        _roomID = "";
        _slideNums.clear();
        // sendMessage(instruction, "", "", "");
        // sendResponse(socket, instruction, "success", "null", QByteArray::fromStdString(""));
    }
    else if (instruction == "stroke") {
        /*QFile f("test.l");
        if (f.open(QIODevice::Append)) {
            f.write(buffer);
            f.write("dfghjklö\n");
            f.close();
        }*/

        //
        // TODO: seperate strokes and refresh them every time - first try
        //
        /*QFile f("save.strokes");
        if (f.open(QIODevice::Append)) {
            f.write(buffer.toStdString().c_str());
        }
        QStringList list = QString::fromStdString(buffer.toStdString()).split("stroke:");
        //std::cout << list.size() << std::endl;
        //std::cout << list[0].toStdString() << " " << list[1].toStdString() << std::endl;
        list.removeAt(0);
        //QPixmap* img = _origImage;

        emit clearDraw();
        //emit setSlide(img);

        for (int i = 0; i < list.size(); i++) {
            QString line = list[i];
            line = line.left(line.lastIndexOf(":"));
            QVector<QPoint> points;
            QStringList l = line.split(":");
            for (int j = 0; j < l.size(); j += 2) {
                QPoint p(l[j].toInt(), l[j+1].toInt());
                points.push_back(p);
            }
            QPolygon p(points);

            QImage img = _currentSlide->toImage();
            QPainter painter(&img);
            painter.setPen(QPen(QColor::fromRgb(0,0,0), 2, Qt::SolidLine, Qt::RoundCap,
                                Qt::RoundJoin));
            painter.drawPolyline(p);
            _lines.push_back(p);
            QPixmap* pix = new QPixmap();
            pix->fromImage(img);
            // pix->save("n" + QString::number(i) + ".jpg");
            emit setSlide(pix);

            points.clear();
        }*/
        //_currentSlide->save("test.jpg");
        //emit setSlide(_currentSlide);

        StrokeRenderer* renderer = new StrokeRenderer();
        renderer->loadImage(_origImage->toImage());
        QImage newImg = renderer->render(QString::fromStdString(buffer.toStdString()));
        QPixmap p = QPixmap::fromImage(newImg);
        emit setSlide(&p);
    }
    else if (instruction == "black") {
        if (arg1 == "true") {
            std::cout << "black" << std::endl;
            emit setBlack();
        } else {
            emit restoreSlide();
            //std::cout << "blackevtl" << std::endl;
        }
    }
    else if (instruction == "registerClient") {
        std::cout << "registered! " << arg2.toStdString() << std::endl;
        _clientID = arg2.toInt();
    }
    else if (instruction == "connect") {
        std::cout << "connected to room: " << arg1.toStdString() << std::endl;
        sendMessage("connected", "success", arg1, "");
        emit makePresentation();
    }
    else if (instruction == "prepareScreenshare") {
        QString localhostname =  QHostInfo::localHostName();
        QString localhostIP;
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
                localhostIP = address.toString();
        }
        /*QString localMacAddress;
        QString localNetmask;
        foreach (const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces()) {
            foreach (const QNetworkAddressEntry& entry, networkInterface.addressEntries()) {
                if (entry.ip().toString() == localhostIP) {
                    localMacAddress = networkInterface.hardwareAddress();
                    localNetmask = entry.netmask().toString();
                    break;
                }
            }
        }
        std::cout << "Localhost name: " << localhostname.toStdString() << std::endl;
        std::cout << "IP = " << localhostIP.toStdString() << std::endl;
        std::cout << "MAC = " << localMacAddress.toStdString() << std::endl;
        std::cout << "Netmask = " << localNetmask.toStdString() << std::endl;*/
        _sServer = new ScreenshareServer(this);
        connect(_sServer, &ScreenshareServer::setFrame, this, &Network::setSlide);
        connect(_sServer, &ScreenshareServer::updateSize, this, &Network::updateSize);
        sendMessage("getClientIP", arg1, localhostIP, "");
    }
    else if (instruction == "endScreenshare") {
        _sServer->_server->close();
        _sServer->deleteLater();
    }
    else {
        std::cout << "Couldn´t decode message - " << instruction.toStdString() << std::endl;
    }
}

void Network::sendResponse(QTcpSocket* socket, QString instruction, QString arg1, QString arg2, QByteArray data) {
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_5);

    QByteArray header;
    header.prepend(QString("instruction:" + instruction + ",arg1:" + arg1 + ",arg2:" + arg2 + ";").toUtf8());
    header.resize(128);

    // std::cout << "sent - " << header.toStdString() << std::endl;

    QByteArray byteArray = data;
    byteArray.prepend(header);

    socketStream << byteArray;
}

void Network::updateSize() {
    emit getSize();
    _sServer->_size = QSize(_size.width(), _size.height());
}

void Network::sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data) {
    //socket.connectToHost(QString::fromStdString(_hostname), _port);
    if (_socket->isOpen() && _socket->isValid()) {
        QDataStream socketStream(_socket);
        socketStream.setVersion(QDataStream::Qt_5_12);

        QByteArray header;
        header.prepend(QString::fromStdString("instruction:" + instruction.toStdString() + ",arg1:" + arg1.toStdString() + ",args2:" + arg2.toStdString() + ";").toUtf8());
        header.resize(128);

        QByteArray byteArray = data;
        byteArray.prepend(header);

        socketStream << byteArray;
    } else {
        QMessageBox::warning(new QWidget(), "Keine Verbindung zum Server!", "Es kann keine Verbindung zum Server hergestellt werden. Starten Sie die App neu und probieren Sie es erneut.", QMessageBox::Ok);
        exit(0);
    }
}

void Network::sendMessage(QTcpSocket* socket, QString message) {
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_5);

    QByteArray header;
    header.prepend(QString("fileType:messagse,fileName:null,fileSize:0;").toUtf8());
    header.resize(128);

    QByteArray byteArray = message.toUtf8();
    byteArray.prepend(header);

    socketStream << byteArray;
}
