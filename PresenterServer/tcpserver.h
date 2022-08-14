#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

#include "room.h"
#include "Database/CDB.h"
#include "Database/CDBFileHandler.h"
#include "Database/CDBRow.h"
#include "strokehandler.h"
#include "filehandler.h"
#include "client.h"
#include "MobileServer/mobileserver.h"
#include "slidemanager.h"
#include "checksystem.h"
#include "dbmanager.h"

class MobileServer;
class SlideManager;
class CheckSystem;
class DbManager;

class TcpServer : public QObject
{
    Q_OBJECT
public:
    friend class MobileServer;
    friend class SlideManager;
    friend class CheckSystem;

    explicit TcpServer(QObject *parent = nullptr);
    void startServer();
    void loadDatabases();
    bool setPresentation(std::string roomName, std::string pres);
    void rebootServer();
    ~TcpServer();

signals:

public slots:
    void newConnection();
    void testImage();
    void closeRoom();

    bool resetPassword(int ID, QString newPW);
private:
    friend class testTcpServer;

    //QSqlDatabase* _db;

    QTcpServer *_server;
    std::vector<Room*> _rooms;
    //CDB* _user;
    //CDB* _presentations;
    // CDB* _clients;
    //CDB* _files;
    QMap<std::string, std::string> _currentFiles;
    Strokehandler* _sh;
    FileHandler* _fh;
    QVector<Client*> _activeClients;
    CheckSystem* _checkSys;
    DbManager* _dbm;

    // Networkoperations
    void readSocket();
    void sendMessage(QTcpSocket* socket, QString message);
    void sendResponse(QTcpSocket* socket, QString instruction, QString success, QString arg1, QString arg2, QByteArray data);
    void sendMessage(QTcpSocket* socket, QString instruction, QString arg1, QString arg2, QByteArray data);
    bool checkLogin(QString username, QString password);

    // Roomoperations
    Room* getRoom(QString room);
    QString createRoom(QString username);

    // Slideoperations / Presentationoperations
    void sendSlide(QString filename, QString roomID);
    QByteArray prepareSlide(QString file);
    QString getPresentationString(QString username);
    QString getFolderName(int presentation);

    // other
    std::string getRandom(int l);
    MobileServer* _mobile;
    SlideManager* _sManager;





    /*
     * Settings implementieren und in XML speichern
     *
     */

    QString _internetAddress = "srv-lange.de";





    // User Management
    bool changeUser(int ID, QString username, QString firstname, QString lastname/*, /*QString password*/, bool admin);
    bool addUser(QString username, QString firstname, QString lastname, QString password, bool admin);
    bool deleteUser(int ID);
};

#endif // TCPSERVER_H
