#include "homewindow.h"
#include "ui_homewindow.h"
#include "settingsdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include "Admin/checksysdialog.h"

HomeWindow::HomeWindow(QWidget *parent, Control* c) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    _c = c;
    ui->pushButton->setVisible(false);
    ui->BtnCheckSys->setVisible(false);
    if (!_c->_net->_isAdmin) {
        ui->BtnServerSettings->setVisible(true);
        ui->BtnServerSettings->setText("Bugs melden");
    } else {
        ui->BtnServerSettings->setVisible(true);
    }
    connect(ui->BtnSettings, &QPushButton::clicked, this, &HomeWindow::BtnSettingsClicked);
    connect(ui->BtnServerSettings, &QPushButton::clicked, this, &HomeWindow::BtnServerSettingsClicked);
    connect(ui->BtnStart, &QPushButton::clicked, this, &HomeWindow::BtnStartClicked);
    connect(ui->BtnDat, &QPushButton::clicked, this, &HomeWindow::BtnDatClicked);
    connect(ui->pushButton, &QPushButton::clicked, this, &HomeWindow::BtnRebootClicked);
    connect(ui->BtnCheckSys, &QPushButton::clicked, this, &HomeWindow::BtnCheckSysClicked);
}

void HomeWindow::BtnCheckSysClicked() {
    CheckSysDialog* dialog = new CheckSysDialog(_c->_net, this);
    dialog->exec();
}

void HomeWindow::BtnSettingsClicked() {
    if (_ignoreClicks) {
        return;
    }
    if (!_isAdminSettings) {
        SettingsDialog* sd = new SettingsDialog(this);
        sd->setTexts(_c->_serverName, _c->_serverIP);
        sd->exec();
        if (sd->_rejected) {
            //exit(0);
            delete sd;
            this->show();
            return;
        }
        QTcpSocket* socket = new QTcpSocket();
        socket->connectToHost(sd->_serverIP, 8083);
        setCursor(Qt::WaitCursor);
        _ignoreClicks = true;
        if (socket->waitForConnected(2000)) {
            QMessageBox::information(this, "Einstellungen korrekt!", "Die Einstellungen wurde erfolgreich überprüft und werden nun gespeichert. Für das Inkrafttreten ist allerdings ein Neustart der App erforderlich.", QMessageBox::Ok);
            _c->saveSettings("settings.xml", sd->_serverName, sd->_serverIP);
            checkDone();
        } else {
            QMessageBox::warning(this, "Einstellungen nicht korrekt!", "Die vorgenommen Änderungen sind nicht korrekt, es konnte mit diesen keine Verbindungen zum Server hergestellt werden. Bitte überprüfen Sie ihre Eingaben und versuchen Sie es erneut.", QMessageBox::Ok);
            checkDone();
        }
        delete sd;
    } else {
        ui->BtnDat->setText("Dateien verwalten");
        ui->BtnServerSettings->setText("Servereinstellungen");
        ui->BtnSettings->setText("Einstellungen");
        ui->BtnStart->setText("Präsentation starten");
        ui->label->setText("Dashboard");
        ui->pushButton->setVisible(false);
        ui->BtnCheckSys->setVisible(false);
        setAdmin();
        _isAdminSettings = false;
    }
}

void HomeWindow::setAdmin() {
    if (!_c->_net->_isAdmin) {
        ui->BtnServerSettings->setVisible(true);
        ui->BtnServerSettings->setText("Bugs melden");
    } else {
        ui->BtnServerSettings->setVisible(true);
        ui->BtnServerSettings->setText("Servereinstellungen");
    }
}

void HomeWindow::setRemote()
{
    ui->BtnStart->setEnabled(false);
    ui->BtnStart->setText("Remote Modus aktiviert!");
    ui->BtnStart->setStyleSheet("background-color: grey;");
}

void HomeWindow::checkDone() {
    setCursor(Qt::ArrowCursor);
    _ignoreClicks = false;
}

void HomeWindow::BtnServerSettingsClicked() {
    if (_ignoreClicks) {
        return;
    }
    if (_c->_net->_isAdmin) {
        ui->label->setText("Servereinstellungen");
        if (!_isAdminSettings) {
            _isAdminSettings = true;
            ui->BtnDat->setText("Benutzer verwalten");
            ui->BtnStart->setText("Clients (Presenter) verwalten");
            //ui->BtnServerSettings->setVisible(false);
            ui->BtnServerSettings->setText("Bugs melden");
            ui->BtnSettings->setText("Zurück");
//            QPushButton* btn = new QPushButton(this);
//            btn->setText("Server neustarten");
//            btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
//            ui->gridLayout->addWidget(btn, 1, 0, Qt::AlignCenter);
            ui->pushButton->setVisible(true);
            ui->BtnCheckSys->setVisible(true);
        } else {
            QMessageBox::information(this, "Bugs melden", "Bitte melden Sie auftretende Bugs an folgende Email-Adresse: maximilian-sangerhausen@gmx.de");
        }
    } else {
        QMessageBox::information(this, "Bugs melden", "Bitte melden Sie auftretende Bugs an folgende Email-Adresse: maximilian-sangerhausen@gmx.de");
    }
}

void HomeWindow::BtnStartClicked() {
    if (_ignoreClicks) {
        return;
    }
    if (!_isAdminSettings) {
        _c->_net->sendMessage("getPresentations", QString::fromStdString(_c->_net->_username), "arg2", QByteArray::fromStdString(""));
        _c->_pres->showNormal();
        this->hide();
    }
}

void HomeWindow::reloadDat() {
    _c->_fm->_isFileDialog = false;
    _c->_fm->disconnect();
    delete _c->_fm;
    _c->_fm = new FileManager(this, _c);
    connect(_c->_fm, &FileManager::reload, this, &HomeWindow::reloadDat);
    connect(_c->_net, &Network::gotFileList, _c->_fm, &FileManager::createFileList);
    // _c->_net->sendMessage("getFileNames", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString(""));
    _c->_fm->showNormal();
}

void HomeWindow::BtnDatClicked() {
    if (_ignoreClicks) {
        return;
    }
    if (!_isAdminSettings) {
        _c->_net->sendMessage("getFileNames", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString(""));
        _c->_fm->showNormal();
        this->hide();
    }
    if (_isAdminSettings) {
        _c->_um = new UserManager(this, _c);
        _c->_net->sendMessage("getUserList", QString::fromStdString(_c->_net->_id), "", "");
        // _c->_um->makeUserList();
        _c->_um->showNormal();
        _c->_hw->hide();
    }
}
void HomeWindow::BtnRebootClicked() {
    if (QMessageBox::question(this, "Neustart ausführen?", "Bei einem Neustart des Servers können Daten verloren gehen und alle Apps vom Typs Sender, welche mit dem Server verbunden sind, müssen neu gestartet werden. Sind Sie sich sicher, dass Sie fortfahren wollen?") == QMessageBox::Yes){
        _c->_net->sendMessage("rebootServer", "", "", "");
        QMessageBox::information(this, "Neustart erfolgreich!", "Der Neustart des Servers konnte erfolgreich ausgeführt werden. Damit die App wieder vollumfänglich funktioniert muss sie neu gestartet werden.");
        exit(0);
    }
}

void HomeWindow::closeEvent(QCloseEvent *e) {
    if (_c->_fm != nullptr) {
        _c->_fm->deleteLater();
    }
    if (_c->_pres != nullptr) {
        _c->_pres->deleteLater();
    }
    if (_c->_presScreen){
        _c->_presScreen->deleteLater();
    }
    if (_c->_pw != nullptr){
        _c->_pw->deleteLater();
    }
    if (_c->_ph != nullptr){
        _c->_ph->deleteLater();
    }
    QTcpSocket* socket = new QTcpSocket();
    socket->connectToHost(_c->_serverIP, 8083);
    if (socket->waitForConnected(2000)) {
        socket->close();
        socket->deleteLater();
        _c->_net->sendMessage("closeRoom", QString::fromStdString(_c->_net->_id), "", QByteArray::fromStdString(""));
    } else {
        delete _c->_net;
        exit(0);
    }
}

HomeWindow::~HomeWindow()
{
    delete ui;
}
