#include "resetpassworddialog.h"
#include "ui_resetpassworddialog.h"
#include <iostream>

ResetPasswordDialog::ResetPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResetPasswordDialog)
{
    ui->setupUi(this);
    ui->label_4->setVisible(false);
    connect(ui->pushButton, &QPushButton::clicked, this, &ResetPasswordDialog::btnCancelClicked);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &ResetPasswordDialog::btnSaveClicked);
}

void ResetPasswordDialog::btnCancelClicked() {
    //std::cout << "cancel" << std::endl;
    close();
}

void ResetPasswordDialog::btnSaveClicked() {
    //std::cout << "save" << std::endl;
    _le1 = ui->lineEdit->text();
    _le2 = ui->lineEdit_2->text();
    // _accepted = true;
    // close();
    if (_le1 == _le2) {
        _accepted = true;
        close();
    } else {
        ui->label_4->setVisible(true);
        ui->lineEdit->setFocus();
    }
}

ResetPasswordDialog::~ResetPasswordDialog()
{
    delete ui;
}
