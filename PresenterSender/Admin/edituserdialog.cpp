#include "edituserdialog.h"
#include "ui_edituserdialog.h"

EditUserDialog::EditUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditUserDialog)
{
    ui->setupUi(this);
    connect(ui->BtnReset, &QPushButton::clicked, this, &EditUserDialog::btnResetPwClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &EditUserDialog::btnSaveClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &EditUserDialog::btnDeleteClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &EditUserDialog::btnCancelClicked);

    ui->LePasswd->setVisible(false);
    ui->label_5->setVisible(false);
}

EditUserDialog::~EditUserDialog()
{
    delete ui;
}

void EditUserDialog::modeAddUser() {
    ui->LePasswd->setVisible(true);
    ui->label_5->setVisible(true);
    ui->btnDelete->setVisible(false);
    ui->BtnReset->setVisible(false);
    _addMode = true;
}

void EditUserDialog::setInformation(QString username, QString firstname, QString lastname, bool admin)
{
    _username = username;
    _firstname = firstname;
    _lastname = lastname;
    _isAdmin = admin;

    ui->LeFirstName->setText(_firstname);
    ui->LeLastName->setText(_lastname);
    ui->LeUsername->setText(_username);
    ui->cBAdmin->setChecked(_isAdmin);
}

void EditUserDialog::btnDeleteClicked()
{
    _operation = "delete";
    _hasChanged = true;
    close();
}

void EditUserDialog::btnSaveClicked()
{
    if (!_addMode) {
        QString username = ui->LeUsername->text();
        QString firstname = ui->LeFirstName->text();
        QString lastname = ui->LeLastName->text();
        bool admin = ui->cBAdmin->isChecked();
        if (_username != username || _firstname != firstname || _lastname != lastname || _isAdmin != admin) {
            std::cout << "Change username settings" << std::endl;
            _hasChanged = true;
            _username = username;
            _firstname = firstname;
            _lastname = lastname;
            _isAdmin = admin;
            _operation = "save";
            close();
        } else {
            std::cout << "Nothing has changed" << std::endl;
            close();
        }
    } else {
        _hasChanged = true;
        _username = ui->LeUsername->text();
        _firstname = ui->LeFirstName->text();
        _lastname = ui->LeLastName->text();
        _isAdmin = ui->cBAdmin->isChecked();
        _password = ui->LePasswd->text();
        _operation = "new";
        close();
    }
}

void EditUserDialog::btnCancelClicked()
{
    close();
}

void EditUserDialog::btnResetPwClicked()
{
    ResetPasswordDialog* dlg = new ResetPasswordDialog();
    if (dlg->exec() == 0)
    {
        if (dlg->_accepted && dlg->_le1 == dlg->_le2) {
            _hasChanged = true;
            _operation = "reset";
            _password = dlg->_le1;
            dlg->deleteLater();
            close();
        }
    }
}
