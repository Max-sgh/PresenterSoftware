#ifndef RESETPASSWORDDIALOG_H
#define RESETPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class ResetPasswordDialog;
}

class ResetPasswordDialog : public QDialog
{
    Q_OBJECT

friend class EditUserDialog;

public:
    explicit ResetPasswordDialog(QWidget *parent = nullptr);
    ~ResetPasswordDialog();

public slots:
    void btnSaveClicked();
    void btnCancelClicked();

private:
    Ui::ResetPasswordDialog *ui;
    QString _le1;
    QString _le2;
    bool _accepted = false;
};

#endif // RESETPASSWORDDIALOG_H
