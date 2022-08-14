#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QList>
#include <QCloseEvent>
#include <QImage>
#include "Filemanager/fileitem.h"
// #include "mainwindow.h"
// #include "control.h"

class Control;

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent, std::string hostname, int port, Control* c);
    ~Network();
    bool connectToHost();
    bool checkServer();
    void login(std::string username, std::string password);
    bool initializeServer();
    bool createRoom();
    Control* _c;
    std::string _id;
    void getPreviews();
    void testImg();
    QTcpSocket* _socket;
    void sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data);
    std::vector<QString> _clientNames;
    bool _shouldClose = false;
    bool _isConnected = false;
    bool _unsavedStrokes = false;
    std::string _username;
    bool _isAdmin = false;
    QImage _origImage;

public slots:
    void loginSuccess(QString completed);
    void gotPreview();
    void nextPreview(int n);

private:
    std::string _hostname;
    std::string _password;
    int _port;
    void getPreview(std::string name);
    std::vector<QPixmap*> _newPixes;
    std::vector<QString> _newNames;
    int _uploadConfirmed = 0;
    QTcpSocket* _remoteSocket;

signals:
    void preparationSuccess();
    void gotAllPreviews();
    void selectedPres();
    void clientConnected();
    void error(QString errorMsg);
    void closeEvent(QCloseEvent* event);
    void gotFileList();
    void isAdmin();
    void changeFileStatus(int id, FileItem::Status status);
    void setRemote();
    void singleCheckDone(QString action, QString status);

private slots:
    void readSocket();
    void readRemoteSocket();
};

#endif // NETWORK_H
