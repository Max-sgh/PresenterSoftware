#include "checksysdialog.h"
#include "ui_checksysdialog.h"
#include <iostream>
#include <QMovie>

CheckSysDialog::CheckSysDialog(Network* net, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckSysDialog)
{
    ui->setupUi(this);
    ui->BtnWeiter->setVisible(false);
    _net = net;

    connect(_net, &Network::singleCheckDone, this, &CheckSysDialog::checkDone);

    QList<CheckItem*> list;
    _items << new CheckItem("Benutzerdatenbank überprüfen", "checkUserDB", this)
         << new CheckItem("Präsentationsdatenbank überprüfen", "checkPresDB", this)
         << new CheckItem("Dateiendatenbank überprüfen", "checkFilesDB", this)
         << new CheckItem("Zugriff aus dem Internet", "checkInternetAccess", this);
    createList(list);
    _net->sendMessage("check", _items[0]->_action, "", "");
}

void CheckSysDialog::checkDone(QString action, QString status) {
    _checked++;
    std::cout << action.toStdString() << std::endl;
    for (int i = 0; i < _items.size(); i++) {
        if (_items[i]->_action == action) {
            if (status == "success") {
                _items[i]->_status = CheckItem::Status::Success;
            } else {
                std::cout << status.toStdString() << std::endl;
                _items[i]->_status = CheckItem::Status::Error;
            }
        }
    }
    createList(_items);
    if (_checked == _items.size()) {
        finished();
    } else {
        nextElement();
    }
}

void CheckSysDialog::nextElement() {
    _net->sendMessage("check", _items[_checked]->_action, "", "");
}

void CheckSysDialog::finished() {

}

void CheckSysDialog::createList(QList<CheckItem*> list) {
    ui->scrollArea->takeWidget();
    QWidget* container = new QWidget(this);
    QVBoxLayout* vlayout = new QVBoxLayout();
    for (int i = 0; i < list.size(); i++) {
        QHBoxLayout* layout = new QHBoxLayout();

        QLabel* name = new QLabel(list[i]->_name);
        QSpacerItem* spacer = new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QLabel* status = new QLabel("status");

        /*QMovie* movie = new QMovie(":icons/spinner.gif");
        status->setMovie(movie);
        movie->start();*/
        // status->setPixmap(QPixmap(":icons/error.jpg").scaled(40,40));
        switch (list[i]->_status){
            case CheckItem::Loading:
                //QMovie* movie = new QMovie(":icons/spinner.gif");
                status->setMovie(new QMovie(":icons/spinner2.gif"));
                // movie->start();
                status->movie()->start();
                status->movie()->setScaledSize(QSize(30,30));
                break;
            case CheckItem::Error:
                status->setPixmap(QPixmap(":icons/error.jpg").scaled(30,30));
                break;
            case CheckItem::Success:
                status->setPixmap(QPixmap(":icons/ok.jpg").scaled(30,30));
                break;
            default:
                break;
        }


        layout->addWidget(name);
        layout->addSpacerItem(spacer);
        layout->addWidget(status);
        QWidget* w = new QWidget();
        w->setLayout(layout);
        vlayout->addWidget(w);
    }
    container->setLayout(vlayout);
    vlayout->addSpacerItem(new QSpacerItem(40,20,QSizePolicy::Minimum, QSizePolicy::Expanding));
    ui->scrollArea->setWidget(container);
}

CheckSysDialog::~CheckSysDialog()
{
    delete ui;
}
