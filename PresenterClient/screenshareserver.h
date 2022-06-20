#ifndef SCREENSHARESERVER_H
#define SCREENSHARESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QSize>

class ScreenshareServer : public QObject
{
    Q_OBJECT
public:
    friend class Network;
    explicit ScreenshareServer(QObject *parent = nullptr);

    void sendMessage(QTcpSocket* socket, QString instruction, QString arg1, QString arg2, QByteArray data);

public slots:
    void newConnection();

signals:
  //  void errorToServer(QString error);
    void setFrame(QPixmap*);
    void updateSize();

private:
    void readSocket();
    QTcpServer* _server;
    QSize _size;
    QPixmap* _lastFrame;

signals:

};

#endif // SCREENSHARESERVER_H
