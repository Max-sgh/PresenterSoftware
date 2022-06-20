#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(this, SIGNAL(accepted()), this, SLOT(buttonAccepted()));
    connect(this, SIGNAL(rejected()), this, SLOT(buttonRejected()));
    _rejected = false;
}

void SettingsDialog::setTexts(QString serverName, QString serverIP) {
    ui->edit_serverIP->setText(serverIP);
    ui->edit_serverName->setText(serverName);
}

void SettingsDialog::buttonRejected() {
    _rejected = true;
}

void SettingsDialog::buttonAccepted() {
    _serverIP = ui->edit_serverIP->text();
    _serverName = ui->edit_serverName->text();
    _rejected = false;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
