#ifndef EDITUSERDIALOG_H
#define EDITUSERDIALOG_H

#include <QDialog>
#include "usermanager.h"
#include "resetpassworddialog.h"

namespace Ui {
class EditUserDialog;
}

class ResetPasswordDialog;

class EditUserDialog : public QDialog
{
    Q_OBJECT

    friend class UserManager;
public:
    explicit EditUserDialog(QWidget *parent = nullptr);
    ~EditUserDialog();
    void setInformation(QString, QString, QString, bool);
    int _id;
    void modeAddUser();

public slots:
    void btnDeleteClicked();
    void btnSaveClicked();
    void btnCancelClicked();
    void btnResetPwClicked();

private:
    Ui::EditUserDialog *ui;
    QString _username;
    QString _firstname;
    QString _lastname;
    bool _isAdmin;
    QString _operation;
    bool _hasChanged = false;
    bool _addMode = false;
    QString _password;
};

#endif // EDITUSERDIALOG_H
