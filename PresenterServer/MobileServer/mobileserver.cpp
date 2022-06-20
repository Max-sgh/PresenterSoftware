#include "mobileserver.h"
#include <iostream>

MobileServer::MobileServer(QObject *parent, TcpServer* origServer)
    : QObject{parent}
{
    _mainServer = origServer;
    std::cout << "Starting Server for mobile application..." << std::endl;
    _server = new QTcpServer(this);
    connect(_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if(!_server->listen(QHostAddress::Any, 8084))
    {
        std::cout << "Server for mobile connection could not start" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Server for mobile connection started" << std::endl;
    }
}

void MobileServer::newConnection()
{
    QTcpSocket *socket = _server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &MobileServer::readSocket);
    std::cout << "connected" << std::endl;
    socket->write("connected\n\r");
    //socket->flush();
}

void MobileServer::readSocket() {
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QString message = socket->readLine();
    QString instruction = message.split(";")[0];
    QString content = message.split(";")[1];

    if (instruction == "Login") {
        std::cout << content.toStdString() << std::endl;
        QString username = content.split(":")[0];
        QString password = content.split(":")[1];
        if (_mainServer->checkLogin(username, password)) {
            std::cout << "success" << std::endl;
            QString id = _mainServer->createRoom(username);
            std::cout << "Mobile Room - " << id.toStdString() << std::endl;
            socket->write(QByteArray::fromStdString(
                              QString("Login;success;" + id + "$$"
                                      + _mainServer->getPresentationString(username) + "\n\r")
                              .toStdString()));
        } else {
            std::cout << "failed" << std::endl;
            socket->write("Login;failed\n\r");
            //socket->flush();
        }
    } else {
        std::cout << "Instruction: " << instruction.toStdString() << std::endl;
    }

}
