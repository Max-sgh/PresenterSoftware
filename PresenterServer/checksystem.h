#ifndef CHECKSYSTEM_H
#define CHECKSYSTEM_H

#include <QObject>
#include "tcpserver.h"

class TcpServer;

class CheckSystem : public QObject
{
    Q_OBJECT
public:
    explicit CheckSystem(TcpServer* server, QObject *parent = nullptr);
    ~CheckSystem();
    QString checkUserDB();
    QString checkPresDB();
    QString checkFilesDB();
    QString checkInternetAccess();

signals:

private:
    TcpServer* _server;
};

#endif // CHECKSYSTEM_H
