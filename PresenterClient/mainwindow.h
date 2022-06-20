#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "network.h"
#include "presentation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Network* _net;
    Presentation* _pres;
    void closeEvent(QCloseEvent*) override;
    void test();

private:
    Ui::MainWindow *ui;
    void restartServer();

public slots:
    void setSlide(QPixmap*);
    void makePresentation();
    void closeRoom();
    void clearDraw();
    void setBlack();
    void restoreSlide();
    void getSize();
};
#endif // MAINWINDOW_H
