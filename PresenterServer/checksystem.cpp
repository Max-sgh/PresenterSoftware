#include "checksystem.h"
#include <iostream>
#include <QFile>
#include <QString>
#include "checkUserClass.h"
#include "checkPresClass.h"
#include "checkFileClass.h"
#include <QDir>
#include <QTcpSocket>
#include <QHostInfo>

CheckSystem::CheckSystem(TcpServer* server, QObject *parent)
    : QObject{parent}
{
    _server = server;
}

CheckSystem::~CheckSystem()
{

}

QString CheckSystem::checkUserDB()
{
    QString filename = QString::fromStdString(_server->_user->getFilename());
    QList<CheckUser*> users;
    QFile f(filename);
    int linenum = 0;
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            const QString line = in.readLine();
            linenum++;
            if (line == "#CDB-Header") {
                continue;
            }
            QString prefix = line.split(":")[1];
            if (prefix == "types") {
                if (!(line.split(":types:")[1] == "i:s:s:s:s:s")) {
                    //std::cout << "Types-Error" << std::endl;
                    return "Types-Error!";
                }
            }
            if (prefix == "row") {
                if (line.split(":").size() != 8) {
                    return "Error on line " + QString::number(linenum) + " - Doesn´t match to the given types";
                }
                QStringList split = line.split(":");
                if (split[2].toInt() < 0) {
                    return "Error on line " + QString::number(linenum) + " - ID cannot be less than 0";
                }
                users << new CheckUser(split[3], split[4], split[5], split[2].toInt());
            }
            if (prefix == "names") {
                if (line != ":names:ID:username:firstName:lastName:password:admin") {
                    return "Error! Coloumn names are not correct.";
                }
            }
        }
        for (int i = 0; i < users.size(); i++) {
            for (int j = 0; j < users.size(); j++) {
                if (users[i]->_id == users[j]->_id && j!=i) {
                    return "Error! User " + QString::number(i) + " and " + QString::number(j) + " are having the same ID.";
                }
                if (users[i]->_username == users[j]->_username && j!=i) {
                    return "Error! User " + QString::number(i) + " and " + QString::number(j) + " are having the same username.";
                }
                if (users[i]->_firstname == users[j]->_firstname && users[i]->_lastname == users[j]->_lastname && j!=i) {
                    return "Error! User " + QString::number(i) + " and " + QString::number(j) + " are having the same first- and lastname.";
                }
            }
        }
    } else {
        return "Error - Opening Databasefile.";
    }
    return "success";
}

QString CheckSystem::checkPresDB()
{
    QString filename = QString::fromStdString(_server->_presentations->getFilename());
    QList<CheckPres*> pres;
    QFile f(filename);
    int linenum = 0;
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            const QString line = in.readLine();
            linenum++;
            if (line == "#CDB-Header") {
                continue;
            }
            if (line.isNull()) {
                continue;
            }
            QString prefix = line.split(":")[1];
            if (prefix == "types") {
                if (!(line.split(":types:")[1] == "i:s:s:s:i")) {
                    //std::cout << "Types-Error" << std::endl;
                    return "Types-Error!";
                }
            }
            if (prefix == "row") {
                if (line.split(":").size() != 7) {
                    return "Error on line " + QString::number(linenum) + " - Doesn´t match to the given types";
                }
                QStringList split = line.split(":");
                if (split[2].toInt() < 0) {
                    return "Error on line " + QString::number(linenum) + " - ID cannot be less than 0";
                }
                pres << new CheckPres(split[3], split[4], split[5], split[2].toInt(), split[6].toInt());
            }
            if (prefix == "names") {
                if (line != ":names:ID:user:folderName:name:numSlides") {
                    return "Error! Coloumn names are not correct.";
                }
            }
        }
        for (int i = 0; i < pres.size(); i++) {
            for (int j = 0; j < pres.size(); j++) {
                if (pres[i]->_id == pres[j]->_id && j!=i) {
                    return "Error! Presentation " + QString::number(i) + " and " + QString::number(j) + " are having the same ID.";
                }
                if (pres[i]->_foldername == pres[j]->_foldername && j!=i) {
                    return "Error! Presentation " + QString::number(i) + " and " + QString::number(j) + " are having the same foldername.";
                }
            }
            QDir dir("Data/" + pres[i]->_foldername);
            QStringList list = dir.entryList(QStringList() << "*.jpg" << "*.JPG" << "*.jpeg", QDir::Files);
            if (list.size() < pres[i]->_numSlides) {
                return "Error! In the directory of Presentation in line " + QString::number(linenum) + " is/are " + QString::number(pres[i]->_numSlides - list.size()) + " slides missing.";
            } else if (list.size() > pres[i]->_numSlides) {
                return "Error! In the directory of Presentation with id " + QString::number(pres[i]->_id) + " is/are " + QString::number(list.size() - pres[i]->_numSlides) + " slides too much.";
            } else {
                continue;
            }
        }
    } else {
        return "Error - Opening Databasefile.";
    }
    return "success";
}

QString CheckSystem::checkFilesDB()
{
    QString filename = QString::fromStdString(_server->_files->getFilename());
    QList<CheckFile*> files;
    QFile f(filename);
    int linenum = 0;
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            const QString line = in.readLine();
            linenum++;
            if (line == "#CDB-Header") {
                continue;
            }
            if (line.isNull()) {
                continue;
            }
            QString prefix = line.split(":")[1];
            if (prefix == "types") {
                if (!(line.split(":types:")[1] == "i:s:s:s:s")) {
                    //std::cout << "Types-Error" << std::endl;
                    return "Types-Error!";
                }
            }
            if (prefix == "row") {
                if (line.split(":").size() != 7) {
                    return "Error on line " + QString::number(linenum) + " - Doesn´t match to the given types";
                }
                QStringList split = line.split(":");
                if (split[2].toInt() < 0) {
                    return "Error on line " + QString::number(linenum) + " - ID cannot be less than 0";
                }
                // QString user, QString foldername, QString name, int id, QString type
                files << new CheckFile(split[3], split[4], split[5], split[2].toInt(), split[6]);
            }
            if (prefix == "names") {
                if (line != ":names:ID:user:folderName:name:type") {
                    return "Error! Coloumn names are not correct.";
                }
            }
        }
        for (int i = 0; i < files.size(); i++) {
            for (int j = 0; j < files.size(); j++) {
                if (files[i]->_id == files[j]->_id && j!=i) {
                    return "Error! Files " + QString::number(i) + " and " + QString::number(j) + " are having the same ID.";
                }
                if (files[i]->_foldername == files[j]->_foldername && j!=i) {
                    return "Error! Files " + QString::number(i) + " and " + QString::number(j) + " are having the same foldername.";
                }
            }
        }
    } else {
        return "Error - Opening Databasefile.";
    }
    return "success";

}

QString CheckSystem::checkInternetAccess()
{
    QTcpSocket* socket = new QTcpSocket(this);
    socket->connectToHost(_server->_internetAddress, 8083);
    if (!socket->waitForConnected(30000)) {
        return "Error! Cannot connect to the server from the internet.";
    } else {
        return "success";
    }
}
