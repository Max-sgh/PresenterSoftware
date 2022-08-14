#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <iostream>
#include <QHostInfo>
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _net = new Network(this);
    connect(_net, &Network::setSlide, this, &MainWindow::setSlide);
    connect(_net, &Network::makePresentation, this, &MainWindow::makePresentation);
    connect(_net, &Network::closeRoom, this, &MainWindow::closeRoom);
    connect(_net, &Network::clearDraw, this, &MainWindow::clearDraw);
    connect(_net, &Network::setBlack, this, &MainWindow::setBlack);
    connect(_net, &Network::restoreSlide, this, &MainWindow::restoreSlide);

    connect(ui->BtnRestart, &QPushButton::clicked, this, &MainWindow::test);


    /*_pres = new Presentation(this);
    _pres->show();
    _pres->open("save.l");*/
}

void MainWindow::restartServer() {
    _net->disconnect();
    _net->deleteLater();
    _net = new Network(this);
    connect(_net, &Network::setSlide, this, &MainWindow::setSlide);
    connect(_net, &Network::makePresentation, this, &MainWindow::makePresentation);
    connect(_net, &Network::closeRoom, this, &MainWindow::closeRoom);
    connect(_net, &Network::clearDraw, _pres, &Presentation::clearDraw);
    connect(_net, &Network::setBlack, this, &MainWindow::setBlack);
    connect(_net, &Network::restoreSlide, this, &MainWindow::restoreSlide);
    connect(_net, &Network::getSize, this, &MainWindow::getSize);
}

void MainWindow::closeRoom() {
    // _pres->deleteLater();
    _pres->close();
    delete _pres;
}

void MainWindow::getSize() {
    _net->_size = _pres->size();
}

void MainWindow::makePresentation() {
    _pres = new Presentation(this);
    _pres->showFullScreen();
    _net->_size = _pres->size();
}

void MainWindow::setSlide(QPixmap* slide) {
    emit _pres->setSlide(slide);
}

void MainWindow::clearDraw() {
    emit _pres->clearDraw();
}

void MainWindow::setBlack()
{
    // std::cout << "black" << std::endl;
    emit _pres->setBlack();
}

void MainWindow::restoreSlide()
{
    emit _pres->restoreSlide();
}

MainWindow::~MainWindow()
{
    _net->sendMessage("deleteClient", _net->_roomID, QString::number(_net->_clientID), "");
    if (_pres != nullptr)
        delete _pres;
    delete _net;
    delete ui;
}

void MainWindow::test() {
    _net->sendMessage("deleteClient", _net->_roomID, QString::number(_net->_clientID), "");
}

void MainWindow::closeEvent(QCloseEvent* e) {
    _net->sendMessage("deleteClient", _net->_roomID, QString::number(_net->_clientID), "");
}

