#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
//#include "presentationhandler.h"
//#include "network.h"
#include "control.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    MainWindow(Control* c);
    ~MainWindow();
    Control* _c;

public slots:
    void closeEvent(QCloseEvent* event) override;
    void backToHome();

private:
    Ui::MainWindow *ui;
    std::string _username;
    std::string _password;
    bool close;
    bool loadSettings();

protected:
    void resizeEvent(QResizeEvent* event) override;
};
#endif // MAINWINDOW_H
