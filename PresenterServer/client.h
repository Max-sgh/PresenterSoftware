#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent, QTcpSocket* socket);
    void sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data);
    void evaluateClientAction(QString action, QString arg2, QByteArray data);

    QTcpSocket* _socket;
    QString _name;
    QString _IP;
    bool _isAvaiable;
    int _ID;

};

#endif // CLIENT_H
