#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QMainWindow>
#include "control.h"

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent, Control* c);
    ~HomeWindow();
    void closeEvent(QCloseEvent* e) override;

private slots:
    void BtnStartClicked();
    void BtnDatClicked();
    void BtnSettingsClicked();
    void BtnServerSettingsClicked();
    void BtnRebootClicked();
    void checkDone();
    void BtnCheckSysClicked();

public slots:
    void reloadDat();
    void setAdmin();
    void setRemote();

private:
    Ui::HomeWindow *ui;
    Control* _c;
    bool _ignoreClicks = false;
    bool _isAdminSettings = false;
};

#endif // HOMEWINDOW_H
