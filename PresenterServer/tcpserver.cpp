#include "tcpserver.h"
#include "room.h"
#include "dbmanager.h"

#include <iostream>
#include <QByteArray>
#include <QDataStream>
#include <QBuffer>
#include <sstream>
#include <fstream>
#include <QImage>
#include <QFile>
#include <QChar>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QSqlQuery>
#include "client.h"
#include "MobileServer/mobileserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    _sh = new Strokehandler(this);
    _fh = new FileHandler(this);
    _dbm = new DbManager(this);

    loadDatabases();
    //deleteUser(5);
    //addUser("efefefe", "fefefef", "efefeff", "1234", true);
    //changeUser(3, "wfeg", "fegrhjthrgf", "efefeff", "12345", true);
    //_fh->convertFile(new Room(), "", _files, _presentations);
}

void TcpServer::rebootServer() {
    std::cout << "reboot..." << std::endl;
    // shutdown all services and delete all temporary data
    _server->disconnect();
    _server->close();
    _server->deleteLater();
    _sh->deleteLater();
    _fh->deleteLater();
    _mobile->disconnect();
    _mobile->_server->close();
    _mobile->deleteLater();
    _sManager->deleteLater();
    _checkSys->deleteLater();
    _rooms.clear();
    //CDBFilehandler* fh = new CDBFilehandler("Data/presentations.cdb");
    //delete fh;
    //fh->saveDB(_presentations);
    /*fh = new CDBFilehandler("Data/files.cdb");
    fh->saveDB(_files);
    delete fh;*/
    //_currentFiles.clear();

    QString path = "Data/Cache";
    QDir dir(path);
    dir.setNameFilters(QStringList() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
    this->disconnect();

    //start everything new
    _sh = new Strokehandler(this);
    _fh = new FileHandler(this);
    startServer();
    std::cout << "rebooted successfully" << std::endl;
}

void TcpServer::startServer() {
    _server = new QTcpServer(this);
    connect(_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if(!_server->listen(QHostAddress::Any, 8083))
    {
        std::cout << "Server could not start" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "Server started" << std::endl;
        loadDatabases();
        _sManager = new SlideManager(this, this);
        _mobile = new MobileServer(this, this);
    }
}

void TcpServer::loadDatabases() {
    CDBFilehandler* fh;// = new CDBFilehandler("Data/presentations.cdb");
    //_presentations = fh->openDB();
    //delete fh;

    /*fh = new CDBFilehandler("Data/users.cdb");
    _user = fh->openDB();
    delete fh;*/

    /*fh = new CDBFilehandler("Data/clients.cdb");
    _clients = fh->openDB();
    delete fh;

    /*fh = new CDBFilehandler("Data/files.cdb");
    _files = fh->openDB();
    delete fh;*/
}

TcpServer::~TcpServer() {
    for (int i = 0; i < static_cast<int>(_rooms.size()) - 1; i++) {
        delete _rooms.at(i);
    }
    //delete _presentations;
    //delete _clients;
    delete _server;
}

void TcpServer::newConnection()
{
    QTcpSocket *socket = _server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &TcpServer::readSocket);
    // sendMessage(socket, "connected");
}

void TcpServer::readSocket() {
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QByteArray buffer;
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    socketStream.startTransaction();
    socketStream >> buffer;

    if (!socketStream.commitTransaction()) {
        // std::cout << "waiting for Data!" << std::endl;
        return;
    }

    QString header = buffer.mid(0,128);
    QString instruction = header.split(",")[0].split(":")[1];
    QString arg1 = header.split(",")[1].split(":")[1];
    QString arg2 = header.split(",")[2].split(":")[1].split(";")[0];
    //std::cout << header.toStdString() << std::endl;

    buffer = buffer.mid(128);
    // SQLITE-ready
    if (instruction == "login") {
        if (checkLogin(arg1, arg2)) {
            bool isAdmin = false;
            /*std::vector<DatabaseRow> rows = _user->getRowsByValue("username", arg1.toStdString());
            if (QString::fromStdString(rows[0]._fields[5].s) == "true") {
                isAdmin = true;
            }*/

            /*QSqlDatabase db = _dbm->getDb();
            QSqlQuery qry = db.exec("SELECT admin FROM users WHERE username = '" + arg1 + "'");
            int results = 0;
            if (!qry.lastError().isValid()) {
                while (qry.next()) {
                    results++;
                }
            } else {
                std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
            }
            if (results == -1 || results > 1) {
                std::cout << "Error! " << results << std::endl;
            }
            qry.first();
            if (qry.value("admin").toString() == "true") {
                isAdmin = true;
            }
            db.close();*/

            // Definition: ret[0] = error (bool); ret[1] = errorString/numResults
            QList<QVariant>* list = _dbm->singleSelect("SELECT admin FROM users WHERE username = '" + arg1 + "'");
            if (list->at(0).toBool() == false) {
                std::cerr << "DB-Error: " << list->at(1).toString().toStdString() << std::endl;
            }
            if (list->at(1).toInt() != 1) {
                std::cerr << "DB-Error" << std::endl;
            } else if (list->at(2).toString() == "true") {
                isAdmin = true;
            }
            delete list;

            sendResponse(socket, instruction, "success", isAdmin ? "true" : "false", "null", QByteArray::fromStdString(/*data.toStdString()*/ "null"));
        } else {
            sendResponse(socket, instruction, "failed", "null", "null", QByteArray::fromStdString("Error <Incorrect Username or Password>"));
        }
    }
    // SQLITE-ready
    else if (instruction == "getPresentations") {
        QString data = getPresentationString(arg1);
        if (data != "no presentations")
            sendResponse(socket, instruction, "success", "null", "null", QByteArray::fromStdString(data.toStdString()));
        else
            sendResponse(socket, instruction, "failed", "null", "null", QByteArray::fromStdString("No Presentations available"));
    }

    else if (instruction == "syncNetworkInformation") {
        QString localhostname =  QHostInfo::localHostName();
        QString localhostIP;
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
                localhostIP = address.toString();
        }
        QString localMacAddress;
        QString localNetmask;
        foreach (const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces()) {
            foreach (const QNetworkAddressEntry& entry, networkInterface.addressEntries()) {
                if (entry.ip().toString() == localhostIP) {
                    localMacAddress = networkInterface.hardwareAddress();
                    localNetmask = entry.netmask().toString();
                    break;
                }
            }
        }
        sendResponse(socket, instruction, "success", localhostIP, "", QByteArray::fromStdString(localMacAddress.toStdString()));
    }

    else if (instruction == "checkForRemote") {
        QString localhostname =  QHostInfo::localHostName();
        QString localhostIP;
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
                localhostIP = address.toString();
        }
        QString localMacAddress;
        foreach (const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces()) {
            foreach (const QNetworkAddressEntry& entry, networkInterface.addressEntries()) {
                if (entry.ip().toString() == localhostIP) {
                    localMacAddress = networkInterface.hardwareAddress();
                    break;
                }
            }
        }
        if (QString::fromStdString(buffer.toStdString()) == localMacAddress) {
            std::cout << "success" << std::endl;
            sendResponse(socket, instruction, "success", "", "", "");
        } else {
            std::cout << buffer.toStdString() << " " << localMacAddress.toStdString() << std::endl;
            std::cout << "failed" << std::endl;
            sendResponse(socket, instruction, "failed", "", "", "");
        }
    }

    else if (instruction == "createRoom") {
        QString id = createRoom(arg1);
        sendResponse(socket, instruction, "success", id, "null" "null", QByteArray::fromStdString("null"));
    }
    // SQLITE-ready
    else if (instruction == "getPreview") {
        std::cout << instruction.toStdString() << " - " << arg2.toInt() << std::endl;
        /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("ID", arg2.toInt());
        DatabaseRow row = rows[0];
        QString file = "Data/" + QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s) + "/Slide1.jpg";*/

        /*QSqlDatabase db = _dbm->getDb();
        QSqlQuery qry = db.exec("SELECT folderName FROM presentations WHERE ID='" + arg2 + "'");
        int results = 0;
        if (!qry.lastError().isValid()) {
            while (qry.next()) {
                results++;
            }
        } else {
            std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
        }
        if (results == -1 || results > 1) {
            std::cout << "Error! " << results << std::endl;
        }
        qry.first();
        QString folderName = qry.value("folderName").toString();
        QString file = "Data/" + folderName + "/Slide1.jpg";
        db.close();*/
        // Definition: ret[0] = error (bool); ret[1] = errorString/numResults
        QList<QVariant>* list = _dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + arg2 + "'");
        if (list->at(0).toBool() == false) {
            std::cerr << "DB-Error: " << list->at(1).toString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failed", arg2, "null", "DB-Error");
            return;
        }
        if (list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: More than one result." << std::endl;
            sendResponse(socket, instruction, "failed", arg2, "null", "DB-Error");
            return;
        }
        QString folderName = list->at(2).toString();
        delete list;
        QString file = "Data/" + folderName + "/Slide1.jpg";

        QFile f(file);
        if (f.open(QIODevice::ReadOnly)) {
            sendResponse(socket, "getPreview", "success", arg2, "null", f.readAll());
            f.close();
        }
        else {
            std::cerr << "Cannot open file - " << file.toStdString() << " - " << f.errorString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failed", arg2, "null", "Cannot open File");
        }
    }

    else if (instruction == "setPresentation") {
        std::cout << instruction.toStdString() << " - " << arg2.toStdString() << std::endl;
        QString pres = arg2;
        QString room = arg1;
        if (setPresentation(room.toStdString(), pres.toStdString())) {
            //std::vector<DatabaseRow> clients = _clients->getRowsByValue("available", "true");

            QString clientsFormatted;
            /*if (clients.size() == 0)
                clientsFormatted = "No Clients available";
            int num = -1;

            for (int i = 0; i < (int)clients.size(); i++){
                if (!(QString::fromStdString(clients[i]._fields[3].s) == "false")) {
                    clientsFormatted += QString::fromStdString(clients[i]._fields[1].s) + ",";
                    num++;
                }
            }*/
            QList<QVariant>* list = _dbm->singleSelect("SELECT name FROM clients WHERE available='true';");
            if (list->at(0).toBool() == false) {
                std::cerr << "DB-Error: TcpServer::readSocket - setPresentation " << list->at(1).toString().toStdString() << std::endl;
                sendResponse(socket, instruction, "failure", "null", "null", QByteArray::fromStdString("DB-Error"));
                delete list;
                return;
            }
            int num = list->at(1).toInt();
            if (num == 0) {
                sendResponse(socket, instruction, "failure", "null", "null", QByteArray::fromStdString("no avaiable"));
                std::cout << "Currently there are no Clients avaiable" << std::endl;
                delete list;
                return;
            }
            for (int i = 2; i < list->count(); i++) {
                clientsFormatted += list->at(i).toString() + ",";
            }
            if (clientsFormatted.at(clientsFormatted.size() - 1) == ",")
                clientsFormatted = clientsFormatted.left(clientsFormatted.lastIndexOf(QChar(',')));
            //std::cout << clientsFormatted.toStdString() << std::endl;
            if (num == -1) {
                std::cout << "Currently there are no Clients avaiable" << std::endl;
                sendResponse(socket, instruction, "failure", "null", "null", QByteArray::fromStdString("no avaiable"));
                delete list;
                return;
            }
            delete list;
            Room* r = getRoom(room);
            r->_currentSlide = -1;
            _sh->openStrokes(r, getFolderName(r->_presID));

            sendResponse(socket, instruction, "success", "null", "null", QByteArray::fromStdString(clientsFormatted.toStdString()));
        } else {
            sendResponse(socket, instruction, "failure", "null", "null", QByteArray::fromStdString("null"));
        }
    }

    else if (instruction == "setClient") {
        QString room = arg1;
        // client z.B. "Raum 320"
        QString client = arg2;
        //
        // TODO: check if the client is already connected in the time.
        //
        Room* r = getRoom(room);
        QList<QVariant>* list = _dbm->singleSelect("SELECT IP FROM clients WHERE name='" + client + "';");
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - setClient " << list->at(1).toString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failure", "Fehler!", "", "Fehler in der Datenbank! (möglicherweise existieren mehrere Clients mit dem selben Namen.)");
            delete list;
            return;
        }
        r->_clientIP = list->at(2).toString();
        //r->_clientIP = QString::fromStdString(_clients->getFieldOfRowByName(_clients->getRowNumber("name", client.toStdString()), "IP").s);
        r->_clientName = client;
        /*r->_clientSocket = new QTcpSocket();
        r->_clientSocket->connectToHost(r->_clientIP, 8081);
        if (r->_clientSocket->waitForConnected(2000)) {
            if (r->_clientSocket->isOpen()) {
                connect(r->_clientSocket, &QTcpSocket::readyRead, r, &Room::readSocket);
                r->_response = socket;
                _clients->changeFieldInRow(_clients->getRowNumber("name", client.toStdString()),"available", "false");
                std::cout << "Connected to Client" << std::endl;
            }
        } else {
            std::cout << "Can´t connect to Client: " << r->_clientIP.toStdString() << std::endl;
            sendResponse(socket, instruction, "failure", "Fehler! Zeitüberschreitung", "", QByteArray::fromStdString("Der Client ist nicht erreichbar oder die Verbindung ist zu schwach, um eine Verbindung herzustellen."));
            r->_clientSocket->close();
            return;
        }*/
        Client* cl;
        for (int i = 0; i < _activeClients.length(); i++) {
            if (_activeClients[i]->_name == client) {
                cl = _activeClients[i];
                break;
            }
        }
        if (cl == nullptr) {
            //Error
            sendResponse(socket, instruction, "failed", "", "", "Could not find Client!");
            return;
        }
        r->_response = socket;
        r->_clientSocket = cl->_socket;
        r->_client = cl;
        r->_isConnected = true;
        r->_currentSlide = -1;
        connect(r->_clientSocket, &QTcpSocket::readyRead, r, &Room::readSocket);
        //_clients->changeFieldInRow(_clients->getRowNumber("name", client.toStdString()),"available", "false");
        int numEffectedRows = _dbm->update("UPDATE clients SET available='false' WHERE name='" + client + "';");
        if (numEffectedRows != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - setClient " << std::endl;
        }
        cl->sendMessage("connect", room, "", "");
        std::cout << "Connected to Client" << std::endl;
    }
    else if (instruction == "nextSlide") {
        Room* r = getRoom(arg1);
        _sManager->nextSlide(r, buffer, this);

        /*Room* r = getRoom(arg1);
        bool hasStrokes = _sh->checkForStrokes(QString::fromStdString(r->_roomID), r->_currentSlide + 1);
        if (r->_currentSlide == -1) {
            std::vector<DatabaseRow> rows = _presentations->getRowsByValue("name", r->_presentation);
            DatabaseRow row = rows[0];
            int num = 1;

            QString file = "Data/" + QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s) + "/Slide1.jpg";
            QByteArray b = prepareSlide(file);
            if (QString::fromStdString(b.toStdString()) == "failed"){
                if (hasStrokes)
                    sendResponse(socket, "Slide", "failed", QString::number(num), "true", b);
                else
                    sendResponse(socket, "Slide", "failed", QString::number(num), "false", b);
                return;
            }

            sendResponse(socket, "Slide", "success", QString::number(num), _sh->checkForStrokes(QString::fromStdString(r->_roomID), 1) ? "true" : "false", b);
            if (r->sendSlide(file, num)) {
                r->_currentSlide = 1;
            }
        }
        else {
            int num = r->_currentSlide;
            QString strokes = QString::fromStdString(buffer.toStdString());
            if (strokes != "empty") {
                if (_sh->checkForStrokes(QString::fromStdString(r->_roomID), r->_currentSlide)) {
                    _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), r->_currentSlide, &strokes);
                    // std::cout << "changed Strokes for Slide #" << num << std::endl;
                }
                else{
                    _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), r->_currentSlide, &strokes);
                    // std::cout << "set Strokes for Slide #" + QString::number(num).toStdString() << std::endl;
                }
            }
            if (num + 1 > r->_maxSlides) {
                sendResponse(socket, "Slide", "success", "last", "", "");
                return;
            }
            std::vector<DatabaseRow> rows = _presentations->getRowsByValue("name", r->_presentation);
            DatabaseRow row = rows[0];
            QString file = "Data/" + QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num + 1) + ".jpg";

            QByteArray b = prepareSlide(file);
            if (QString::fromStdString(b.toStdString()) == "failed"){
                sendResponse(socket, "Slide", "failed", QString::number(num), hasStrokes ? "true" : "false" , b);
                std::cout << "failed" << std::endl;
                return;
            }

            sendResponse(socket, "Slide", "success", QString::number(num), hasStrokes ? "true" : "false", b);
            if (r->sendSlide(file, num)) {
                r->_currentSlide++;
            }
        }*/
    }
    else if (instruction == "prevSlide") {
        Room* r = getRoom(arg1);
        _sManager->prevSlide(r, buffer, this);

        /*Room* r = getRoom(arg1);
        int num = r->_currentSlide;
        if (num - 1 < 1) {
            return;
        }
        QString strokes = QString::fromStdString(buffer.toStdString());
        if (strokes != "empty") {
            if (_sh->checkForStrokes(QString::fromStdString(r->_roomID), r->_currentSlide)) {
                _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), r->_currentSlide, &strokes);
                // std::cout << "changed Strokes for Slide #" << num << std::endl;
            }
            else{
                _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), r->_currentSlide, &strokes);
                // std::cout << "set Strokes for Slide #" + QString::number(num).toStdString() << std::endl;
            }
        }

        std::vector<DatabaseRow> rows = _presentations->getRowsByValue("ID", r->_presID);
        if (rows.size() > 1)
            std::cerr << "Error in Database! There are 2 or more presentations with the same ID." << std::endl;
        DatabaseRow row = rows[0];

        QString file = "Data/" + QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num - 1) + ".jpg";
        QByteArray b = prepareSlide(file);
        if (QString::fromStdString(b.toStdString()) == "failed"){
            sendResponse(socket, "Slide", "failed", QString::number(num), _sh->checkForStrokes(QString::fromStdString(r->_roomID), num - 1) ? "true" : "false", b);
            std::cout << "failed" << std::endl;
            return;
        }

        sendResponse(socket, "Slide", "success", QString::number(num), _sh->checkForStrokes(QString::fromStdString(r->_roomID), num - 1) ? "true" : "false", b);
        if (r->sendSlide(file, num)) {
            r->_currentSlide--;
        }*/
    }
    else if (instruction == "closeRoom") {
        std::cout << instruction.toStdString() << std::endl;
        Room* room = getRoom(arg1);
        if (room->_isConnected) {
            connect(room, &Room::closeRoom, this, &TcpServer::closeRoom);
            room->sendMessage("close", "null", "null", QByteArray::fromStdString("null"));
        }
        sendResponse(socket, instruction, "success", "", "", QByteArray::fromStdString(""));
        if (!room->_presentation.empty())
            _sh->cleanCache(room, getFolderName(room->_presID));
    }
    else if (instruction == "closeClient") {
        Room* room = getRoom(arg1);
        if (room->_isConnected) {
            room->sendMessage("close", "null", "null", QByteArray::fromStdString("null"));
            //_clients->changeFieldInRow(_clients->getRowNumber("name", room->_clientName.toStdString()),"available", "true");
            int numEffectedRows = _dbm->update("UPDATE clients SET available='true' WHERE name='" + room->_clientName + "';");
            if (numEffectedRows != 1) {
                std::cerr << "DB-Error: TcpServer::readSocket - setClient " << std::endl;
            }
            room->_isConnected = false;
            if (!room->_presentation.empty())
                _sh->cleanCache(room, getFolderName(room->_presID));
        }
        sendResponse(socket, instruction, "success", "", "", "");
    }
    else if (instruction == "black") {
        Room* r = getRoom(arg1);
        if (!r->_isBlack){
            r->sendMessage(instruction, "true", "", "");
            r->_isBlack = true;
            sendResponse(socket, instruction, "success", "true", "", "");
        } else {
            int num = r->_currentSlide - 1;
            bool hasStrokes = _sh->checkForStrokes(QString::fromStdString(r->_roomID), num);
            QString strokes = QString::fromStdString(buffer.toStdString());
            if (strokes != "empty") {
                if (_sh->checkForStrokes(QString::fromStdString(r->_roomID), num)) {
                    _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), num, &strokes);
                    // std::cout << "changed Strokes for Slide #" << num << std::endl;
                }
                else{
                    _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), num, &strokes);
                    // std::cout << "set Strokes for Slide #" + QString::number(num).toStdString() << std::endl;
                }
            }


            /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("ID", r->_presID);
            if (rows.size() > 1)
                std::cerr << "Error in Database! There are 2 or more presentations with the same ID." << std::endl;
            DatabaseRow row = rows[0];
            QString file = "Data/" + QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num + 1) + ".jpg";*/

            /*QSqlDatabase db = _dbm->getDb();
            QSqlQuery qry = db.exec("SELECT folderName FROM presentations WHERE ID='" + QString::number(r->_presID) + "'");
            int results = 0;
            if (!qry.lastError().isValid()) {
                while (qry.next()) {
                    results++;
                }
            } else {
                std::cerr << "Error! " << qry.lastError().text().toStdString() << std::endl;
            }
            if (results == -1 || results > 1) {
                std::cerr << "Error! " << results << std::endl;
            }
            qry.first();
            std::cout << qry.value("folderName").toString().toStdString() << std::endl;
            QString file = "Data/" + qry.value("folderName").toString() + "/Slide" + QString::number(num + 1) + ".jpg";*/

            QList<QVariant>* list = _dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + QString::number(r->_presID) + "'");
            if (list->at(0).toBool() == false) {
                std::cerr << "DB-Error: " << list->at(1).toString().toStdString() << std::endl;
            }
            if (list->at(1).toInt() != 1) {
                std::cerr << "DB-Error: More than one results" << std::endl;
            }
            QString file = "Data/" + list->at(2).toString() + "/Slide" + QString::number(num+1) + ".jpg";
            delete list;
            QByteArray b = prepareSlide(file);
            sendResponse(socket, "Slide", "success", QString::number(num), hasStrokes ? "true" : "false", b);
            if (r->sendSlide(file, num)) {
            }
            r->_isBlack = false;
            //db.close();
        }
    }
    else if (instruction == "stroke") {
        Room* r = getRoom(arg1);
        r->sendMessage(instruction, arg2, "", buffer);
    }
    else if (instruction == "getStrokes") {
        Room* r = getRoom(arg1);
        std::cout << "get Strokes for Slide#" << r->_currentSlide/*.toStdString()*/ << std::endl;
        if (_sh->checkForStrokes(QString::fromStdString(r->_roomID), r->_currentSlide)) {
            sendResponse(socket, instruction, "success", arg2, "null", QByteArray::fromStdString(_sh->getStrokes(QString::fromStdString(r->_roomID), r->_currentSlide).toStdString()));
        } else {
            sendResponse(socket, instruction, "failed", arg2, "null", QByteArray::fromStdString("no Strokes!"));
        }
    }
    else if (instruction == "saveStrokes") {
        Room* r = getRoom(arg1);
        QString strokes = QString::fromStdString(buffer.toStdString());
        if (strokes != "empty") {
            if (_sh->checkForStrokes(QString::fromStdString(r->_roomID), r->_currentSlide)) {
                _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), r->_currentSlide, &strokes);
            }
            else{
                _sh->saveStrokesInCache(QString::fromStdString(r->_roomID), r->_currentSlide, &strokes);
            }
        }
        _sh->saveStrokes(r, getFolderName(r->_presID));
        sendResponse(socket, instruction, "success", "", "", QByteArray::fromStdString(""));
    }
    else if (instruction == "getFileNames") {
        Room* room = getRoom(arg1);
        /*std::vector<DatabaseRow> rows = _files->getRowsByValue("user", room->_username);
        if (rows.size() == 0) {
            sendResponse(socket, instruction, "failed", "", "", QByteArray::fromStdString("noFiles"));
            return;
        }
        QString out;
        for (int i = 0; i < (int)rows.size(); i++) {
            out.append(QString::fromStdString(rows[i]._fields[_files->getIndexByName("name")].s) + "->" + QString::fromStdString(rows[i]._fields[_files->getIndexByName("type")].s) + "->" + QString::number(rows[i]._fields[_files->getIndexByName("ID")].i) + ";");
        }*/
        QList<QList<QVariant>*>* list = _dbm->multipleSelect("SELECT name, type, ID FROM files WHERE user='" + QString::fromStdString(room->_username) + "';", 3);
        QString out;
        if (list->at(0)->at(0).toBool() == false) {
            std::cerr << "DB-Error: TcpServer::readSocket - getFileNames " << list->at(0)->at(1).toString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failed", "", "", QByteArray::fromStdString(list->at(0)->at(1).toString().toStdString()));
            return;
        }
        for (int i = 1; i < list->count(); i++) {
            QList<QVariant>* l = list->at(i);
            out.append(l->at(0).toString() + "->" + l->at(1).toString() + "->" + l->at(2).toString() + ";");
        }
        out = out.left(out.lastIndexOf(";"));
        delete list;
        sendResponse(socket, instruction, "success", "arg1", "null", QByteArray::fromStdString(out.toStdString()));
    }
    else if (instruction == "uploadFile") {
        std::cout << instruction.toStdString() << " - " << arg2.toStdString() << std::endl;
        QFile f("Data/Cache/" + arg2);
        QString room = arg1.split("$$")[0];
        int id = arg1.split("$$")[1].toInt();
        if (f.open(QIODevice::WriteOnly)) {
            f.write(buffer);
            f.close();
            if (_fh->convertFile(getRoom(room), "Data/Cache/" + arg2, _dbm))
                sendResponse(socket, instruction, "success", QString::number(id), "", QByteArray::fromStdString(""));
            else
                sendResponse(socket, instruction, "failed", QString::number(id), "", QByteArray::fromStdString("Cannot Convert File!"));
        } else {
            std::cout << f.errorString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failed", QString::number(id), "", QByteArray::fromStdString(f.errorString().toStdString()));
        }
    }
    else if (instruction == "deleteFile") {
        int id = arg2.toInt();
        Room* r = getRoom(arg1);
        QList<QList<QVariant>*>* list = _dbm->multipleSelect("SELECT user, folderName FROM files WHERE ID='" + QString::number(id) + "';", 2);
        if (list->at(0)->at(0).toBool() == false || list->at(0)->at(1).toInt() != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - deleteFile " << list->at(0)->at(1).toString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failed", "", "", "");
        }
        if (list->at(1)->at(0).toString() != QString::fromStdString(r->_username)) {
            std::cout << "Error! The File doesn´t belong to the user." << std::endl;
            sendResponse(socket, instruction, "failed", "", "", "");
        } else {
            QString dirname = list->at(1)->at(1).toString();
            bool b = _fh->deleteFile(dirname, _dbm);
            sendResponse(socket, instruction, b ? "success" : "failed", "", "", "");
        }
        delete list;
        /*std::vector<DatabaseRow> rows = _files->getRowsByValue("ID", id);
        if (QString::fromStdString(rows[0]._fields[1].s) != QString::fromStdString(r->_username)) {
            std::cerr << "Error! The File doesn´t belong to the user." << std::endl;
            sendResponse(socket, instruction, "failed", "", "", "");
        } else {
            QString dirname = QString::fromStdString(rows[0]._fields[2].s);
            bool b = _fh->deleteFile(dirname, _files, _dbm);
            sendResponse(socket, instruction, b ? "success" : "failed", "", "", "");
        }*/
    }
    else if (instruction == "exportToPDF") {
        // Room* r = getRoom(arg1);
        //QString filename = QString::fromStdString(r->_presentation);
        //QString filename = QString::fromStdString(_files->getRowsByValue("ID", arg2.toInt())[0]._fields[3].s);
        QList<QVariant>* list = _dbm->singleSelect("SELECT folderName FROM files WHERE ID='" + QString::number(arg2.toInt()) + "';");
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - exportToPDF " << list->at(1).toString().toStdString() << std::endl;
        }
        QString filename = list->at(2).toString();
        delete list;
        std::cout << filename.toStdString() << std::endl;
        /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("folderName", _files->getRowsByValue("ID", arg2.toInt())[0]._fields[2].s);
        QString dir = "Data/" + QString::fromStdString(rows[0]._fields[2].s);
        int numSlides = rows[0]._fields[4].i;*/
        list = _dbm->singleSelect("SELECT numSlides FROM presentations WHERE ID='" + QString::number(arg2.toInt()) + "'");
        //QList<QList<QVariant>*>*
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: " << list->at(1).toString().toStdString() << std::endl;
        }
        //QString dir = "Data/" + list->at(2).toString();
        QString dir = "Data/" + filename;
        int numSlides = list->at(2).toInt();
        std::cout << list->at(2).toInt() << std::endl;
        delete list;

        QVector<QImage> imgs;
        for (int i = 0; i < numSlides; i++) {
            int num = i+1;
            QString currentFile = dir + "/Slide" + QString::number(num) + ".jpg";
            QImage img(currentFile);
            if (QFile(dir + "/Slide" + QString::number(num) + ".strokes").exists()) {
                QFile f(dir + "/Slide" + QString::number(num) + ".strokes");
                if (f.open(QIODevice::ReadOnly)) {
                    QString data = f.readAll();
                    f.close();
                    imgs << _fh->renderSlide(img, data);
                }
            } else{
                imgs << img;
            }
        }
        _fh->exportToPDF(imgs, dir + "/" + filename + "_export.pdf");
        QFile f(dir + "/" + filename + "_export.pdf");
        if (f.open(QIODevice::ReadOnly)) {
            sendResponse(socket, instruction, "success", dir + "/" + filename + "_export.pdf", "", f.readAll());
        } else {
            std::cout << f.errorString().toStdString() << std::endl;
        }
    }
    else if (instruction == "rebootServer") {
        rebootServer();
    }
    else if (instruction == "registerClient") {
        //
        // Note: socket is here not the Sender, it´s the client at Startup
        // create new Client() and save the socket in class - Clients()??? with save and search etc.
        Client* client = new Client(this, socket);
        client->_name = arg1;
        client->_IP = arg2;
        client->_isAvaiable = true;
        client->_socket = socket;
        _activeClients.append(client);
        std::cout << "Client registered!" << std::endl;
        // get information of client and save them in Client object - in contructor of Client

        // register Client in database as avaiable
        /*DatabaseRow* row = new DatabaseRow();
        int i = _clients->getNumRows();
        client->_ID = i+1;
        row->createField(i+1, ""); // ID
        row->createField(0, client->_name.toStdString()); // name
        row->createField(0, client->_IP.toStdString()); // IP
        row->createField(0, "true"); // avaiable
        _clients->addRow(row);
        _clients->print();*/
        int numEffectedRows = _dbm->insert("INSERT INTO clients (name, IP, available) VALUES ('" + client->_name + "','" + client->_IP + "','true')");
        if (numEffectedRows != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - registerClient" << std::endl;
            sendResponse(socket, instruction, "failure", "", "", "");
            return;
        }
        QList<QVariant>* list = _dbm->singleSelect("SELECT ID FROM clients WHERE name='" + client->_name + "' AND IP='" + client->_IP + "';");
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - registerClient" << std::endl;
            sendResponse(socket, instruction, "failure", "", "", "");
            delete list;
            return;
        }
        client->_ID = list->at(2).toInt();
        delete list;
        sendResponse(socket, instruction, "success", QString::number(client->_ID), "", "");
    }
    else if (instruction == "deleteClient") {
        //Room* room = getRoom(arg1);
        int id = arg2.toInt();
        //_clients->deleteRow(_clients->getRowNumber("ID", row));
        int numEffectedRows = _dbm->remove("DELETE FROM clients WHERE ID='" + QString::number(id) + "'");
        if (numEffectedRows != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - deleteClient" << std::endl;
        }
        for (int i = 0; i < _activeClients.length(); i++) {
            if (_activeClients[i]->_ID == id) {
                _activeClients.remove(i);
                _activeClients[i]->deleteLater();
                break;
            }
        }
    }
    else if (instruction == "ClientAction") {
        // search for client from arg1
        // call Method in client to evaluate the instruction
    }
    else if (instruction == "connected") {
        Room* r = getRoom(arg2);
        sendResponse(r->_response, "setClient", "success", "", "", "");
    }
    else if (instruction == "getUserList") {
        std::cout << instruction.toStdString() << std::endl;
        QString returnString = "";
        /*for (int i = 0; i < _user->getDBSize(); i++) {
            returnString += QString::number(_user->getRowByNumber(i)._fields[0].i) + ","
                    + QString::fromStdString(_user->getRowByNumber(i)._fields[1].s) + ","
                    + QString::fromStdString(_user->getRowByNumber(i)._fields[2].s) + ","
                    +  QString::fromStdString(_user->getRowByNumber(i)._fields[3].s) + ","
                    +  QString::fromStdString(_user->getRowByNumber(i)._fields[5].s) + ";";
        }
        returnString.remove(returnString.length() - 1, 1);*/
        /*QSqlDatabase db = _dbm->getDb();
        QSqlQuery query(db);
        query.prepare("SELECT * FROM users");
        if (!query.exec()) {
            std::cerr << "Query Error!: " << query.lastError().text().toStdString() << std::endl;
            exit(1);
        }
        while (query.next()) {
            returnString += query.value(0).toString() + ","
                    + query.value(1).toString() + ","
                    + query.value(2).toString() + ","
                    + query.value(3).toString() + ","
                    + query.value(4).toString() + ","
                    + query.value(5).toString() + ";";
        }*/
        QList<QList<QVariant>*>* list = _dbm->multipleSelect("SELECT ID, username, firstname, lastname, password, admin FROM users", 6);
        if (list->at(0)->at(0).toBool() == false) {
            std::cerr << "DB-Error: " << list->at(0)->at(1).toString().toStdString() << std::endl;
            sendResponse(socket, instruction, "failed", "", "", QByteArray::fromStdString(list->at(0)->at(1).toString().toStdString()));
            return;
        }
        for (int i = 1; i < list->count(); i++) {
            QList<QVariant>* l = list->at(i);
            returnString += l->at(0).toString() + "," + l->at(1).toString() + "," + l->at(2).toString() + "," + l->at(3).toString() + ","
                    + l->at(4).toString() + "," + l->at(5).toString() + ";";
        }
        returnString.remove(returnString.length() - 1, 1);
        delete list;
        sendResponse(socket, instruction, "success", "", "", QByteArray::fromStdString(returnString.toStdString()));
    }
    /*else if (instruction == "newUser") {
        QStringList list = QString::fromStdString(buffer.toStdString()).split(":");
        addUser(list[0], list[1], list[2], list[3], list[4]=="true" ? true : false);
    }*/
    else if (instruction == "changeUser") {
        QString b = QString::fromStdString(buffer.toStdString());
        int id = b.split(";")[0].toInt();
        QString username = b.split(";")[1];
        QString firstname = b.split(";")[2];
        QString lastname = b.split(";")[3];
        //QString password = b.split(";")[4];
        bool admin = b.split(";")[4] == "true" ? true : false;
        changeUser(id, username, firstname, lastname/*, password*/, admin);
        sendResponse(socket, instruction, "success", "", "", "");
    }
    else if (instruction == "deleteUser") {
        std::cout << "delete User #" << arg2.toStdString() << std::endl;
        deleteUser(arg2.toInt());
        sendResponse(socket, instruction, "success", "", "", "");
    }
    else if (instruction == "newUser") {
        std::cout << "addUser" << std::endl;
        QString b = QString::fromStdString(buffer.toStdString());
        QString username = b.split(";")[0];
        QString firstname = b.split(";")[1];
        QString lastname = b.split(";")[2];
        QString password = b.split(";")[3];
        bool admin = b.split(";")[4] == "true" ? true : false;
        addUser(username, firstname, lastname, password, admin);
        sendResponse(socket, instruction, "success", "", "", "");
    }
    else if (instruction == "resetPassword") {
        std::cout << "resetPassword for User #" << arg2.toStdString() << std::endl;
        if (resetPassword(arg2.toInt(), QString::fromStdString(buffer.toStdString()))) {
            sendResponse(socket, instruction, "success", "", "", "");
        } else {
            sendResponse(socket, instruction, "failure", "", "", "");
        }
    }
    else if (instruction == "prepareScreenshare") {
        Room* room = getRoom(arg1);
        //QTcpSocket* sock = room->_clientSocket;
        room->_client->sendMessage("prepareScreenshare", arg1, "", "");
        room->_response = socket;
    }
    else if (instruction == "getClientIP") {
        Room* room = getRoom(arg1);
        sendResponse(room->_response, "prepareScreenshare", "success", arg2, "", "");
    }
    else if (instruction == "endScreenshare") {
        Room* room = getRoom(arg1);
        room->sendMessage("endScreenshare", "", "", "");
        int num = room->_currentSlide;
        /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("ID", room->_presID);
        DatabaseRow row = rows[0];

        QString file = "Data/" + QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num) + ".jpg";
        */
        QList<QVariant>* list = _dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + QString::number(room->_presID) + "';");
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: TcpServer::readSocket - endScreenshare" << list->at(1).toString().toStdString() << std::endl;
        }
        QString file = "Data/" + list->at(2).toString() + "/Slide" + QString::number(num) + ".jpg";
        delete list;
        QByteArray b = prepareSlide(file);
        if (QString::fromStdString(b.toStdString()) == "failed"){
            //sendResponse(socket, "Slide", "failed", QString::number(num), _sh->checkForStrokes(QString::fromStdString(room->_roomID), num) ? "true" : "false", b);
            std::cout << "failed" << std::endl;
            return;
        }

        //sendResponse(socket, "Slide", "success", QString::number(num), _sh->checkForStrokes(QString::fromStdString(room->_roomID), num) ? "true" : "false", b);
        room->sendSlide(file, num);
        sendResponse(socket, instruction, "success", "", "", "");
    }
    else if (instruction == "check") {
        for (int i = 0; i < 50000000; i++) {}
        if (arg1 == "checkUserDB") {
            _checkSys = new CheckSystem(this, this);
            sendResponse(socket, instruction, _checkSys->checkUserDB(), arg1, "", "");
            _checkSys->deleteLater();
        } else if (arg1 == "checkPresDB") {
            _checkSys = new CheckSystem(this, this);
            sendResponse(socket, instruction, _checkSys->checkPresDB(), arg1, "", "");
            _checkSys->deleteLater();
        } else if (arg1 == "checkFilesDB") {
            _checkSys = new CheckSystem(this, this);
            sendResponse(socket, instruction, _checkSys->checkFilesDB(), arg1, "", "");
            _checkSys->deleteLater();
        } else if (arg1 == "checkInternetAccess") {
            _checkSys = new CheckSystem(this, this);
            sendResponse(socket, instruction, _checkSys->checkInternetAccess(), arg1, "", "");
            _checkSys->deleteLater();
        }
    }
    else {
        std::cout << "Cannot decode message - " << instruction.toStdString() << std::endl;
    }
}

QString TcpServer::getPresentationString(QString username) {
    QString data;
    /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("user", username.toStdString());
    if (rows.size() == 0) {
        return "no presentations";
    }
    QVector<QString> presentations;
    QVector<int> ids;
    for (int i = 0; i < static_cast<int>(rows.size()); i++) {
        struct field f = rows[i]._fields[_presentations->getIndexByName("name")];
        presentations.push_back(QString::fromStdString(f.s));
        ids.push_back(rows[i]._fields[_presentations->getIndexByName("ID")].i);
    }
    for (int i = 0; i < presentations.size(); i++) {
        data += presentations[i] + "," + QString::number(ids[i]) + ";";
    }
    data.remove(data.length() - 1, 1);*/
    // Design: Name,ID;
    /*QSqlDatabase db = _dbm->getDb();
    QSqlQuery qry = db.exec("SELECT ID, name FROM presentations WHERE user='" + username + "'");
    int results = 0;
    if (!qry.lastError().isValid()) {
        while (qry.next()) {
            results++;
            data += qry.value("name").toString() + "," + QString::number(qry.value("ID").toInt()) + ";";
        }
    } else {
        std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
    }
    db.close();*/

    // Definition: < <error; errorString/numResults>; <...>
    QList<QList<QVariant>*>* list = _dbm->multipleSelect("SELECT ID, name FROM presentations WHERE user='" + username + "'", 2);
    if (list->at(0)->at(0).toBool() == false) {
        std::cout << "DB-Error: " << list->at(0)->at(1).toString().toStdString() << std::endl;
    }
    for (int i = 1; i < list->count(); i++) {
        QList<QVariant>* l = list->at(i);
        data += l->at(1).toString() + "," + QString::number(l->at(0).toInt()) + ";";
        //list->removeOne(l);
        delete l;
    }
    data.remove(data.length() - 1, 1);
    delete list;
    return data;
}

QString TcpServer::getFolderName(int presentation) {
    /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("ID", presentation);
    if (rows.size() > 1)
        std::cerr << "Error in Database! There are 2 or more presentations with the same ID." << std::endl;
    DatabaseRow row = rows[0];
    return QString::fromStdString(row._fields[_presentations->getIndexByName("folderName")].s);*/
    QList<QVariant>* list = _dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + QString::number(presentation) + "'");
    if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
        std::cerr << "DB-Error TcpServer::getFolderName : " << list->at(1).toString().toStdString() << std::endl;
        return "";
    }
    QString ret = list->at(2).toString();
    delete list;
    return ret;
}

QString TcpServer::createRoom(QString username) {
    Room* room = new Room();
    room->_username = username.toStdString();
    QString id = QString::fromStdString(room->getID());
    std::cout << "Create Room - " << id.toStdString() << std::endl;
    _rooms.push_back(room);
    return id;
}

void TcpServer::closeRoom() {
    Room* room = reinterpret_cast<Room*>(sender());
    //_clients->changeFieldInRow(_clients->getRowNumber("name", room->_clientName.toStdString()),"available", "true");
    int numEffectedRows = _dbm->update("UPDATE clients SET available='true' WHERE name='" + room->_clientName + "';");
    if (numEffectedRows != 1) {
        std::cerr << "DB-Error: TcpServer::closeRoom" << std::endl;
    }
    for (int i = 0; i < (int)_rooms.size(); i++) {
        if (QString::fromStdString(_rooms[i]->getID()) == QString::fromStdString(room->getID())) {
            _rooms.erase(_rooms.begin() + i);
            break;
        }
    }
    room->disconnect();
    room->deleteLater();
}

bool TcpServer::changeUser(int ID, QString username, QString firstname, QString lastname/*, QString password*/, bool admin){
    /*int i = _user->getRowNumber("ID", ID);
    if (i == -1) {
        return false;
    }
    _user->changeFieldInRow(i, "username", username.toStdString());
    _user->changeFieldInRow(i, "firstName", firstname.toStdString());
    _user->changeFieldInRow(i, "lastName", lastname.toStdString());
    //_user->changeFieldInRow(i, "password", password.toStdString());
    _user->changeFieldInRow(i, "admin", admin ? "true" : "false");
    CDBFilehandler* fh = new CDBFilehandler("Data/users.cdb");
    fh->saveDB(_user);
    delete fh;*/
    int numRowsAffected = _dbm->update("UPDATE users SET username='" + username + "', firstname='" + firstname + "', lastname='" + lastname
                                       + "', admin='" + (admin==true ? "true" : "false") + "' WHERE ID='" + QString::number(ID) + "';");
    if (numRowsAffected != 1) {
        std::cerr << "DB-Error! - TcpServer::changeUser()" << std::endl;
        return false;
    }
    return true;
}

bool TcpServer::addUser(QString username, QString firstname, QString lastname, QString password, bool admin) {
    /*DatabaseRow* row = new DatabaseRow();
    int i = _user->getNumRows() + 1;
    row->createField(i, "");
    row->createField(0, username.toStdString());
    row->createField(0, firstname.toStdString());
    row->createField(0, lastname.toStdString());
    row->createField(0, password.toStdString());
    row->createField(0, admin ? "true" : "false");
    _user->addRow(row);
    CDBFilehandler* fh = new CDBFilehandler("Data/users.cdb");
    fh->saveDB(_user);
    delete fh;*/
    int numEffectedRows = _dbm->insert("INSERT INTO users (username, firstname, lastname, password, admin) VALUES ('" + username
                                       + "', '" + firstname + "', '" + lastname + "', '" + password + "', '" + (admin==true ? "true" : "false") + "');");
    if (numEffectedRows != 1) {
        std::cerr << "DB-Error: TcpServer::addUser" << std::endl;
        return false;
    }
    return true;
}

bool TcpServer::deleteUser(int ID) {
    /*_user->deleteRow(ID);
    CDBFilehandler* fh = new CDBFilehandler("Data/users.cdb");
    fh->saveDB(_user);
    delete fh;*/
    int numEffectedRows = _dbm->remove("DELETE FROM users WHERE ID='" + QString::number(ID) + "'");
    if (numEffectedRows != 1) {
        std::cerr << "DB-Error: TcpServer::deleteUser" << std::endl;
        return false;
    }
    return true;
}

bool TcpServer::resetPassword(int ID, QString newPW) {
    /*int i = _user->getRowNumber("ID", ID);
    if (i == -1) {
        std::cerr << "Error!" << std::endl;
        return false;
    }
    _user->changeFieldInRow(i, "password", newPW.toStdString());
    CDBFilehandler* fh = new CDBFilehandler("Data/users.cdb");
    fh->saveDB(_user);
    delete fh;*/
    int numEffectedRows = _dbm->insert("UPDATE users SET password='" + newPW + "' WHERE ID='" + QString::number(ID) + "';");
    if (numEffectedRows != 1) {
        std::cerr << "DB-Error: TcpServer::resetPassword" << std::endl;
        return false;
    }
    return true;
}

QByteArray TcpServer::prepareSlide(QString file) {
    QFile img(file);
    if (img.open(QIODevice::ReadOnly)) {
        QByteArray b = img.readAll();
        img.close();
        return b;
    }
    else {
        return QByteArray::fromStdString("failed");
    }
}

void TcpServer::testImage() {
    //std::cout << "Hi" << std::endl;
    // sendSlide("Data/0001/Slide4.jpg", QString::fromStdString(reinterpret_cast<Room*>(sender())->_roomID));
}

bool TcpServer::checkLogin(QString username, QString password) {
    std::cout << "checkLogin" << std::endl;

    /*std::vector<DatabaseRow> rows = _user->getRowsByValue("username", username.toStdString());
    if (rows.size() != 1) {
        return false;
    }
    if (QString::fromStdString(rows[0]._fields[4].s) == password) {
        return true;
    }*/
    /*QSqlDatabase db = _dbm->getDb();
    QSqlQuery q(db);
    QSqlQuery qry = db.exec("SELECT ID FROM users WHERE username='" + username + "' AND password='" + password + "'");
    int results = 0;
    if (!qry.lastError().isValid()) {
        while (qry.next()) {
            results++;
        }
    } else {
        std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
        return false;
    }
    if (results == -1 || results > 1) {
        std::cout << "Error! " << results << std::endl;
        return false;
    }
    db.close();*/
    // Definition: ret[0] = error (bool); ret[1] = numResults
    QList<QVariant>* list = _dbm->singleSelect("SELECT ID FROM users WHERE username='" + username + "' AND password='" + password + "'");
    if (list->at(0).toBool() == false) {
        std::cerr << "DB-Error" << std::endl;
    }
    if (list->at(1).toInt() != 1) {
        delete list;
        return false;
    }
    delete list;
    return true;
}

Room* TcpServer::getRoom(QString room) {
    Room* r;
    for (int i = 0; i < (int)_rooms.size(); i++) {
        if (_rooms[i]->_roomID.compare(room.toStdString()) == 0) {
            r = _rooms[i];
        }
    }
    if (r == nullptr){
        std::cout << "Cannot find Room" << std::endl;
        return new Room();
    }
    return r;
}

void TcpServer::sendResponse(QTcpSocket* socket, QString instruction, QString success, QString arg1, QString arg2, QByteArray data) {
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("instruction:" + instruction + ",success:" + success + ",arg1:" + arg1 + ",arg2:" + arg2 + ";").toUtf8());
    header.resize(128);


    QByteArray byteArray = data;
    byteArray.prepend(header);

    socketStream << byteArray;
}

void TcpServer::sendMessage(QTcpSocket* socket, QString message) {
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("fileType:messagse,fileName:null,fileSize:0;").toUtf8());
    header.resize(128);

    QByteArray byteArray = message.toUtf8();
    byteArray.prepend(header);

    socketStream << byteArray;
}

void TcpServer::sendMessage(QTcpSocket* socket, QString instruction, QString arg1, QString arg2, QByteArray data) {
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString("instruction:" + instruction + ",arg1:" + arg1 + ",arg2:" + arg2 + ";").toUtf8());
    header.resize(128);

    QByteArray byteArray = data;
    byteArray.prepend(header);

    socketStream << byteArray;
}

std::string TcpServer::getRandom(int l) {
    char caracters[] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    size_t maxindex = sizeof(caracters) - 1;
    std::string s;
    for (int i = 0; i < l; i++) {
        s += (caracters[rand() % maxindex]);
    }
    return s;
}

bool TcpServer::setPresentation(std::string roomName, std::string pres) {
    std::cout << pres << std::endl;
    for (int i = 0; i < static_cast<int>(_rooms.size()); i++) {
        if (_rooms[i]->getID() == roomName) {
            /*std::vector<DatabaseRow> rows = _presentations->getRowsByValue("ID", QString::fromStdString(pres).toInt());
            if (rows.size() > 1)
                std::cerr << "Error in Database! There are 2 or more presentations with the same ID." << std::endl;
            int max = rows[0]._fields[4].i;
            _rooms[i]->_maxSlides = max;
            _rooms[i]->_presentation = pres;
            _rooms[i]->_presID = QString::fromStdString(pres).toInt();
            _rooms[i]->_currentSlide = 1;
            std::cout << _rooms[i]->_maxSlides << " " << _rooms[i]->_presentation << _rooms[i]->_presID << std::endl;*/


            /*QSqlDatabase db = _dbm->getDb();
            QSqlQuery qry = db.exec("SELECT numSlides, name FROM presentations WHERE ID='" + QString::fromStdString(pres) + "'");
            int results = 0;
            if (!qry.lastError().isValid()) {
                while (qry.next()) {
                    results++;
                }
            } else {
                std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
            }
            if (results == -1 || results > 1) {
                std::cerr << "Error in Database! There are 2 or more presentations with the same ID." << std::endl;
            }
            //qry.first();
            _rooms[i]->_maxSlides = qry.value("numSlides").toInt();
            _rooms[i]->_presentation = pres;
            _rooms[i]->_presID = QString::fromStdString(pres).toInt();
            _rooms[i]->_currentSlide = 1;
            db.close();*/

            QList<QList<QVariant>*>* list = _dbm->multipleSelect("SELECT numSlides, name FROM presentations WHERE ID='" + QString::fromStdString(pres) + "'", 2);
            if (list->at(0)->at(0).toBool() == false) {
                std::cerr << "DB-Error: " << list->at(0)->at(1).toString().toStdString() << std::endl;
                return false;
            }
            if (list->at(0)->at(1) != 1) {
                std::cerr << "DB-Error: More than one result! There are 2 or more presentations with the same ID." << std::endl;
                return false;
            }
            _rooms[i]->_maxSlides = list->at(1)->at(0).toInt();
            _rooms[i]->_presentation = pres;
            _rooms[i]->_presID = QString::fromStdString(pres).toInt();
            _rooms[i]->_currentSlide = 1;
            delete list;
            return true;
        }
    }
    return false;
}
