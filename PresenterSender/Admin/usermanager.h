#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QMainWindow>
#include "control.h"

namespace Ui {
class UserManager;
}

class Control;

class UserManager : public QMainWindow
{
    Q_OBJECT

public:
    friend class Network;
    explicit UserManager(QWidget *parent, Control* c);
    ~UserManager();
    void makeUserList();
    void closeEvent(QCloseEvent* e) override;

public slots:
    void btnEditClicked();
    void addButtonClicked();

private:
    Ui::UserManager *ui;
    QString _user;
    Control* _c;
};

#endif // USERMANAGER_H
