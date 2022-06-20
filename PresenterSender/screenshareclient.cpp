#include "screenshareclient.h"
#include "ui_screenshareclient.h"
#include <QMessageBox>
#include <QScreen>
#include <QTimer>
#include <QFile>
#include <QImage>
#include <QBuffer>

ScreenshareClient::ScreenshareClient(QWidget *parent, Control* c, QString ipaddr) :
    QDialog(parent),
    ui(new Ui::ScreenshareClient)
{
    ui->setupUi(this);
    _activated = false;
    _c = c;
    _ip = ipaddr;
    connectToHost();
    QList<QScreen*> screens = QGuiApplication::screens();
    for (int i = 0; i < screens.size(); i++) {
        QString s = screens.at(i)->model() + screens.at(i)->manufacturer();
        ui->comboBox->addItem(s);
    }
    connect(ui->pushButton, &QPushButton::clicked, this, &ScreenshareClient::btnPressed);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &ScreenshareClient::btnBackPressed);
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &ScreenshareClient::sendScreenshot);
}

void ScreenshareClient::btnPressed() {
    if (!_activated) {
        ui->pushButton->setText("Übertragung beenden");
        _screen = ui->comboBox->currentText();
        sendScreenshot();
        _timer->start(50);
        //sendScreenshot();
        _activated = true;
    } else {
        _timer->stop();
        ui->pushButton->setText("Übertragung aktivieren");
        _activated = false;
    }
}

void ScreenshareClient::sendScreenshot() {
    QList<QScreen*> screens = QGuiApplication::screens();
    QScreen* screen;
    for (int i = 0; i < screens.size(); i++) {
        QString s = screens.at(i)->model() + screens.at(i)->manufacturer();
        if (s == _screen) {
            screen = screens[i];
            break;
        }
    }
    auto geom = screen->geometry();
    QPixmap pix = screen->grabWindow(0, geom.x(), geom.y(), geom.width(), geom.height());
    _screenshot = &pix;
    QByteArray barray;
    QBuffer buffer(&barray);
    _screenshot->save(&buffer, "JPG");
    sendMessage("frame", "", "", barray);
}

void ScreenshareClient::btnBackPressed() {
    _timer->stop();
    _c->_net->sendMessage("endScreenshare", QString::fromStdString(_c->_net->_id), "", "");
    _c->_screenClient->hide();
    _c->_pres->showMaximized();
    _timer->deleteLater();
    this->deleteLater();
}

bool ScreenshareClient::connectToHost() {
    _socket = new QTcpSocket();
    _socket->connectToHost(_ip, 8084);
    if (_socket->waitForConnected(2000)) {
        connect(_socket, &QTcpSocket::readyRead, this, &ScreenshareClient::readSocket);
        return true;
    } else {
        return false;
    }
}

void ScreenshareClient::readSocket() {
    QByteArray buffer;
    QDataStream socketStream(_socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    socketStream.startTransaction();
    socketStream >> buffer;

    if (!socketStream.commitTransaction()) {
        // std::cout << "waiting for Data!" << std::endl;
        return;
    }

    QString header = buffer.mid(0,128);
    QString instruction = header.split(",")[0].split(":")[1];

    if (instruction == "connect") {
        sendMessage(instruction, QString::fromStdString(_c->_net->_id), "", "");
        std::cout << instruction.toStdString() << std::endl;
    }
}

void ScreenshareClient::sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data) {
    if (_socket->isOpen()) {
        QDataStream socketStream(_socket);
        socketStream.setVersion(QDataStream::Qt_5_12);

        QByteArray header;
        header.prepend(QString::fromStdString("instruction:" + instruction.toStdString() + ",arg1:" + arg1.toStdString() + ",args2:" + arg2.toStdString() + ";").toUtf8());
        header.resize(128);

        QByteArray byteArray = data;
        byteArray.prepend(header);

        socketStream << byteArray;
    } else {
        QMessageBox::warning(new QWidget(), "Keine Verbindung zum Server!", "Es kann keine Verbindung zum Server hergestellt werden. Starten Sie die App neu und probieren Sie es erneut.", QMessageBox::Ok);
        exit(0);
    }
}

void ScreenshareClient::closeEvent(QCloseEvent* e) {
    _timer->stop();
    _c->_net->sendMessage("endScreenshare", QString::fromStdString(_c->_net->_id), "", "");
    _c->_screenClient->hide();
    _c->_pres->showMaximized();
    _timer->deleteLater();
    this->deleteLater();
}

ScreenshareClient::~ScreenshareClient()
{
    delete ui;
}
