#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpServer>
#include <QPixmap>
#include <QPolygon>
#include "screenshareserver.h"

class Network : public QObject
{
    Q_OBJECT
public:
    friend class MainWindow;
    explicit Network(QObject *parent = nullptr);
    Q_DECL_DEPRECATED void sendMessage(QTcpSocket* socket, QString message);
    void sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data);

private:
    QTcpServer* _server;
    void readSocket();
    std::vector<int> _slideNums;
    std::vector<QPixmap*> _slides;
    Q_DECL_DEPRECATED void sendResponse(QTcpSocket* socket, QString instruction, QString arg1, QString arg2, QByteArray data);
    QString _roomID;
    int _clientID;
    QString _name;
    QPixmap* _currentSlide;
    QPixmap* _origImage;
    QVector<QPolygon> _lines;

    QString name = "Raum1";
    QString IP = "192.168.178.58";
    QTcpSocket* _socket;
    QSize _size;

    ScreenshareServer* _sServer;

public slots:
    Q_DECL_DEPRECATED void newConnection();
    void updateSize();

signals:
    void setSlide(QPixmap*);
    void makePresentation();
    void closeRoom();
    void clearDraw();
    void setBlack();
    void restoreSlide();
    void getSize();

};

#endif // NETWORK_H
