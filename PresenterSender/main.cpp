#include "mainwindow.h"
#include "control.h"
#include "ui_mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    a.setQuitOnLastWindowClosed(false);
    // w.show();
    return a.exec();
}
