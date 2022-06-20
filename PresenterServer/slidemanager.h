#ifndef SLIDEMANAGER_H
#define SLIDEMANAGER_H

#include <QObject>
#include "tcpserver.h"
#include "room.h"

class TcpServer;

class SlideManager : public QObject
{
    Q_OBJECT
public:
    explicit SlideManager(QObject *parent, TcpServer* server);
    void nextSlide(Room* room, QByteArray strokes, TcpServer* _server);
    void prevSlide(Room* room, QByteArray strokes, TcpServer* _server);

private:
    TcpServer* _server;

};

#endif // SLIDEMANAGER_H
