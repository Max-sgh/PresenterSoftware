#ifndef MOBILESERVER_H
#define MOBILESERVER_H

#include <QObject>
#include <QTcpServer>
#include "../tcpserver.h"

class TcpServer;

class MobileServer : public QObject
{
    Q_OBJECT
public:
    friend class TcpServer;
    explicit MobileServer(QObject *parent, TcpServer* origServer);

public slots:
    void newConnection();

private:
    TcpServer* _mainServer;
    QTcpServer* _server;
    void readSocket();

signals:

};

#endif // MOBILESERVER_H
