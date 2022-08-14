#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    friend class MainWindow;
    friend class HomeWindow;
    friend class Network;

public:
    SettingsDialog(QWidget *parent = nullptr);
    void setTexts(QString serverName, QString serverIP);
    QString _serverName;
    QString _serverIP;
    ~SettingsDialog();

private slots:
    void buttonRejected();
    void buttonAccepted();

private:
    Ui::SettingsDialog *ui;
    bool _rejected;
    QLabel* _wait;
};

#endif // SETTINGSDIALOG_H
