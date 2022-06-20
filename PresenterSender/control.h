#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QSplashScreen>
#include "network.h"
#include "mainwindow.h"
#include "Presentation/presentationhandler.h"
#include "Presentation/presentationscreen.h"
#include "Presentation/paintwidget.h"
#include "Presentation/presentation.h"
#include "homewindow.h"
#include "Filemanager/filemanager.h"
#include "Admin/usermanager.h"
#include "screenshareclient.h"

class PresentationHandler;
class Network;
class Presentation;
class PaintWidget;
class Presentation;
class PresentationScreen;
class HomeWindow;
class FileManager;
class UserManager;
class ScreenshareClient;

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    ~Control();
    Network* _net;
    PresentationHandler* _ph;
    Presentation* _pres;
    PresentationScreen* _presScreen;
    PaintWidget* _pw;
    HomeWindow* _hw;
    FileManager* _fm;
    UserManager* _um;
    std::vector<std::string> iterateArgs(std::string origin);
    void saveSettings(QString filename, QString serverName, QString serverIP);
    QString _serverName;
    QString _serverIP;
    QSplashScreen* _splash;
    ScreenshareClient* _screenClient;
    bool _isRemote = false;

signals:
    void showMainWindow();
    void hideMainWindow();

};

#endif // CONTROL_H
