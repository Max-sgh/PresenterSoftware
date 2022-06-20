#ifndef SLIDESHOWHANDLER_H
#define SLIDESHOWHANDLER_H

#include <QObject>
#include "room.h"
#include "tcpserver.h"

class TcpServer;

class SlideshowHandler : public QObject
{
    Q_OBJECT
public:
    explicit SlideshowHandler(QObject *parent, TcpServer* server);
    QByteArray nextSlide(QString room, QString* ret);
    void previousSlide();

private:
    TcpServer* _server;
};

#endif // SLIDESHOWHANDLER_H
