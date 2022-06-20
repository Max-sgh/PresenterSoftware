#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString _username;
    QString _password;
    bool _exit;
    ~LoginDialog();

public slots:
    void buttonAccepted();
    void buttonRejected();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
