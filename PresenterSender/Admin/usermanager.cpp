#include "usermanager.h"
#include "ui_usermanager.h"
#include <QPushButton>
#include "edituserdialog.h"
#include <QMessageBox>

UserManager::UserManager(QWidget *parent, Control* c) :
    QMainWindow(parent),
    ui(new Ui::UserManager)
{
    ui->setupUi(this);
    _c = c;
}

void UserManager::makeUserList()
{
    ui->scrollArea->takeWidget();
    QStringList list = _user.split(";");
    QWidget* w = new QWidget(this);
    QVBoxLayout* v = new QVBoxLayout(this);
    for (int i = 0; i < list.size(); i++) {
        QStringList li = list[i].split(",");
        int id = li[0].toInt();
        QString username = li[1];
        QString firstname = li[2];
        QString lastname = li[3];
        bool admin = li[4] == "true" ? true : false;
        QGridLayout* h = new QGridLayout(this);
        QLabel* l = new QLabel(firstname + " " + lastname);
        QLabel* l2 = new QLabel(username);
        QSpacerItem* spacer = new QSpacerItem(40,20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        QSpacerItem* spacer2 = new QSpacerItem(40,20, QSizePolicy::Minimum, QSizePolicy::Minimum);
        QPushButton* btn = new QPushButton("Bearbeiten");
        btn->setObjectName(list[i]);
        connect(btn, &QPushButton::clicked, this, &UserManager::btnEditClicked);

        h->addWidget(l, 0, 0, 0, 2);
        // h->addSpacerItem(spacer2);
        h->addWidget(l2, 0, 2, 0, 2);
        // h->addSpacerItem(spacer);
        h->addWidget(btn, 0, 5);
        //h->addWidget(p1);
        //h->addWidget(p2);
        //h->addWidget(p3);
        v->addLayout(h);
    }
    QPushButton* addButton = new QPushButton("+");
    v->addWidget(addButton);
    connect(addButton, &QPushButton::clicked, this, &UserManager::addButtonClicked);
    QSpacerItem* sp2 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    v->addSpacerItem(sp2);
    w->setLayout(v);
    ui->scrollArea->setWidget(w);
    w->show();
    setCursor(Qt::ArrowCursor);
}

void UserManager::btnEditClicked() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QStringList list = btn->objectName().split(",");
    EditUserDialog* dlg = new EditUserDialog();
                // username, firstname, lastname, admin
    dlg->setInformation(list[1], list[2], list[3], list[4] == "true" ? true : false);
    dlg->_id = list[0].toInt();
    // dlg.setMode (edit/new)
    if (dlg->exec() == 0) {
        if (dlg->_hasChanged && dlg->_operation == "save") {
            QString data = QString::number(dlg->_id) + ";" + dlg->_username + ";"
                    + dlg->_firstname + ";" + dlg->_lastname + ";";
            data += dlg->_isAdmin == true ? "true" : "false";
            _c->_net->sendMessage("changeUser", QString::fromStdString(_c->_net->_id), "", QByteArray::fromStdString(data.toStdString()));
        }
        else if (dlg->_hasChanged && dlg->_operation == "delete") {
            if (QMessageBox::warning(new QWidget,"Konto löschen?", "Sind Sie sicher, dass Sie das Konto löschen wollen?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                _c->_net->sendMessage("deleteUser", QString::fromStdString(_c->_net->_id), QString::number(dlg->_id), "");
            }
        }
        else if (dlg->_hasChanged && dlg->_operation == "reset") {
            _c->_net->sendMessage("resetPassword", QString::fromStdString(_c->_net->_id), QString::number(dlg->_id), QByteArray::fromStdString(dlg->_password.toStdString()));
        }
    }
}

void UserManager::addButtonClicked() {
    EditUserDialog* dlg = new EditUserDialog();
    dlg->modeAddUser();
    if (dlg->exec() == 0) {
        if (dlg->_hasChanged) {
            QString data = dlg->_username + ";" + dlg->_firstname + ";" + dlg->_lastname + ";" + dlg->_password + ";";
            data += dlg->_isAdmin ? "true" : "false";
            _c->_net->sendMessage("newUser", QString::fromStdString(_c->_net->_id), "", QByteArray::fromStdString(data.toStdString()));
        }
    }
}

void UserManager::closeEvent(QCloseEvent *e) {
    this->hide();
    _c->_hw->show();
    e->ignore();
}

UserManager::~UserManager()
{
    delete ui;
}
