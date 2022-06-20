#ifndef SCREENSHARECLIENT_H
#define SCREENSHARECLIENT_H

#include <QDialog>
#include "control.h"
#include <QTcpSocket>

namespace Ui {
class ScreenshareClient;
}

class Control;

class ScreenshareClient : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenshareClient(QWidget *parent, Control* c, QString ipaddr);
    ~ScreenshareClient();
    void sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data);
    bool connectToHost();
    void readSocket();
    void closeEvent(QCloseEvent*) override;

private slots:
    void btnPressed();
    void btnBackPressed();
    void sendScreenshot();

private:
    Ui::ScreenshareClient *ui;
    Control* _c;
    QString _ip;
    QTcpSocket* _socket;
    bool _activated;
    QString _screen;
    QTimer* _timer;
    QPixmap* _screenshot;
};

#endif // SCREENSHARECLIENT_H
