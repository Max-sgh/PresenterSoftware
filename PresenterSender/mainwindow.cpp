#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "control.h"
#include "logindialog.h"
#include "settingsdialog.h"
#include "dashboard.h"

#include <iostream>
#include <QPixmap>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <sstream>
#include <QWidget>
#include <QFont>
#include <QCloseEvent>
#include <QtXml>
#include <QMessageBox>
#include <QApplication>
#include <QUuid>
#include <QHostInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    _c = new Control(this);

    QPixmap pix(":icons/splashscreen.jpg");
    _c->_splash = new QSplashScreen(pix);
    QFont font = _c->_splash->font();
    font.setPixelSize(20);
    font.setBold(true);
    _c->_splash->setFont(font);
    _c->_splash->showMessage("Initialisiere...", Qt::AlignRight | Qt::AlignBottom, Qt::white);

    _c->_splash->show();

    // Einstellungen laden
    _c->_splash->showMessage("Lade Einstellungen...", Qt::AlignRight | Qt::AlignBottom, Qt::white);
    if (!loadSettings()) {
        delete _c;
        exit(1);
    }

    ui->setupUi(this);
    _c->_net = new Network(this, _c->_serverIP.toStdString(), 8083, _c);
    _c->_net->initializeServer();

    _c->_splash->hide();

    LoginDialog* ld = new LoginDialog(this);
    ld->exec();
    if (ld->_exit) {
        delete _c;
        exit(0);
    }
    _c->_splash->show();
    //std::cout << ld->_username.toStdString() << " " << ld->_password.toStdString() << std::endl;
    _username = ld->_username.toStdString();
    _password = ld->_password.toStdString();

    _username = "lange.m";
    _password = "Rsu3sc123";
    _c->_net->login(_username, _password);

    _c->_ph = new PresentationHandler(new QObject, _c);
    _c->_pres = new Presentation(this, _c);
    _c->_presScreen = new PresentationScreen(_c);
    _c->_hw = new HomeWindow(this, _c);
    _c->_fm = new FileManager(this, _c);

    connect(_c->_net, &Network::closeEvent, this, &MainWindow::closeEvent);
    connect(_c->_net, &Network::error, _c->_pres, &Presentation::error);
    connect(_c->_pres, &Presentation::backToHome, this, &MainWindow::backToHome);
    connect(_c->_net, &Network::gotFileList, _c->_fm, &FileManager::createFileList);
    connect(_c->_fm, &FileManager::reload, _c->_hw, &HomeWindow::reloadDat);
    connect(_c->_net, &Network::isAdmin, _c->_hw, &HomeWindow::setAdmin);
    connect(_c->_net, &Network::setRemote, _c->_hw, &HomeWindow::setRemote);

    //_c->_hw->show();
    // _c->_pres->showNormal();
    /*Dashboard* db = new Dashboard(this);
    db->showNormal();*/
}

bool MainWindow::loadSettings() {
    QDomDocument dom;
    QFile settings("settings.xml");
    if (!settings.exists()) {
        SettingsDialog* sd = new SettingsDialog(this);
        sd->exec();
        if (sd->_rejected) {
            exit(0);
        }
        _c->_serverIP = sd->_serverIP;
        _c->_serverName = sd->_serverName;
        _c->saveSettings("settings.xml", _c->_serverName, _c->_serverIP);

        delete sd;
        return true;
    } else {
        if (!settings.open(QIODevice::ReadOnly)) {
            std::cout << "Error opening Document - " << settings.errorString().toStdString() << std::endl;
            return false;
        }
        if (!dom.setContent(&settings)) {
            std::cout << "Error loading File" << std::endl;
        }
        QDomElement root = dom.firstChildElement();
        QDomNodeList items = root.elementsByTagName("SetItem");
        for (int i = 0; i < items.size(); i++) {
            QDomNode node = items.at(i);
            if (node.isElement()) {
                QDomElement element = node.toElement();
                if (element.attribute("Name") == "serverName") { _c->_serverName = element.attribute("value"); }
                if (element.attribute("Name") == "serverIP") { _c->_serverIP = element.attribute("value"); }
            }
        }
        settings.close();
        return true;
    }
    return false;
}

void MainWindow::backToHome() {
    /*
     *
     * Back to Homemenu einbauen
     *
     */
    // _c->_net->sendMessage("closeClient", QString::fromStdString(_c->_net->_id), "", QByteArray::fromStdString(""));
    _c->_pres->hide();
    _c->_hw->show();

    _c->_pres->disconnect();
    _c->_pres->deleteLater();
    _c->_pres = new Presentation(this, _c);

    _c->_presScreen->disconnect();
    _c->_presScreen->deleteLater();
    _c->_presScreen = new PresentationScreen(_c);

    connect(_c->_net, &Network::error, _c->_pres, &Presentation::error);
    connect(_c->_pres, &Presentation::backToHome, this, &MainWindow::backToHome);
    _c->_ph->_ids.clear();
    _c->_ph->_pixes.clear();
    _c->_ph->_presentations.clear();
    _c->_net->_clientNames.clear();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event){
    if (!_c->_net->_shouldClose && _c->_net->_id.size() > 0 /*&& _c->_net->_isConnected*/) {
        // connect(_c->_net, &Network::closeEvent, this, &MainWindow::closeEvent);
        event->ignore();
    }
}

MainWindow::~MainWindow()
{
    // std::cout << "closeRoom" << std::endl;
    // _c->_net->sendMessage("closeRoom", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString("null"));

    delete _c;
    delete ui;
}
