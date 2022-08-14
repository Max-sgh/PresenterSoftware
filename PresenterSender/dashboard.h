#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QList>
#include "ui/sidewidget.h"

namespace Ui {
class Dashboard;
}

class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard();
    void closeEvent(QCloseEvent*) { exit(0); };

public slots:
    void newActive();

private:
    Ui::Dashboard *ui;
    QWidget* _centralW;
    QLabel* _logo;
    QLabel* _userIcon;
    QWidget* _menubar;
    QWidget* _sideBar;
    QVBoxLayout* _vlayout;
    SideWidget* _currentActive;
    QList<SideWidget*> _widgets;
};

#endif // DASHBOARD_H
