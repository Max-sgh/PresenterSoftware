#include "logindialog.h"
#include "ui_logindialog.h"
#include <iostream>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(buttonAccepted()));
    connect(this, SIGNAL(rejected()), this, SLOT(buttonRejected()));
    _exit = false;
}

void LoginDialog::buttonAccepted() {
    _username = ui->lineEdit->text();
    _password = ui->lineEdit_2->text();
    _exit = false;
}

void LoginDialog::buttonRejected() {
    _exit = true;
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
