#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMap>
#include <string>
#include "client.h"

class Room : public QObject
{
    Q_OBJECT
public:
    explicit Room();
    ~Room();
    int _currentSlide;
    std::string _username;
    std::string _presentation;
    std::string getID();
    std::string _getImgID;
    int _maxSlides;
    std::string _roomID;
    QString _clientIP;
    QTcpSocket* _clientSocket;
    void sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data);
    QTcpSocket* _response;
    void clientConnected();
    QString _clientName;
    void test(QByteArray b);
    bool sendSlide(QString filename, int number);
    bool _isConnected = false;
    bool _isBlack = false;
    Client* _client;
    bool _sendClient = true;
    bool _sendSender = true;
    int _presID;
    //QVector<QString> _currentStrokes;
    //QMap<int, QString> _strokes;

public slots:
    void readSocket();

signals:
    void testImage();
    void closeRoom();

private:
    int _IDLength = 7;
    QString _strokes[10];

};

#endif // ROOM_H
