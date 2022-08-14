#include "slidemanager.h"
#include "strokehandler.h"
#include "filehandler.h"
#include <iostream>

SlideManager::SlideManager(QObject *parent, TcpServer* server)
    : QObject{parent}
{
    _server = server;
}

void SlideManager::nextSlide(Room* room, QByteArray strokes, TcpServer* _server) {
    Strokehandler* sh = _server->_sh;

    bool hasStrokes = _server->_sh->checkForStrokes(QString::fromStdString(room->_roomID), room->_currentSlide + 1);
    if (room->_currentSlide == -1) {
        //std::vector<DatabaseRow> rows = _server->_presentations->getRowsByValue("ID", room->_presID);
        /*if (rows.size() > 1 && room->_sendSender){
            _server->sendResponse(room->_response, "Slide", "failed", "", "", "Fehler! Präsentation kann nicht identifiziert werden.");
            return;
        }
        else if (rows.size() > 1) {
            std::cerr << "Error! Cannot identify presentation" << std::endl;
            return;
        }*/
        //DatabaseRow row = rows[0];
        int num = 1;
        //QString file = "Data/" + QString::fromStdString(row._fields[_server->_presentations->getIndexByName("folderName")].s) + "/Slide1.jpg";
        QList<QVariant>* list = _server->_dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + QString::number(room->_presID) + "';");
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: SlideManager::nextSlide() " << list->at(1).toString().toStdString() << std::endl;
            return;
        }
        QString file = "Data/" + list->at(2).toString() + "/Slide1.jpg";
        delete list;
        QByteArray b = _server->prepareSlide(file);
        if (QString::fromStdString(b.toStdString()) == "failed" && room->_sendSender){
            if (hasStrokes)
                _server->sendResponse(room->_response, "Slide", "failed", QString::number(num), "true", b);
            else
                _server->sendResponse(room->_response, "Slide", "failed", QString::number(num), "false", b);
            return;
        }

        _server->sendResponse(room->_response, "Slide", "success", QString::number(num), _server->_sh->checkForStrokes(QString::fromStdString(room->_roomID), 1) ? "true" : "false", b);
        if (room->sendSlide(file, num)) {
            room->_currentSlide = 1;
        }
    }
    else {
        int num = room->_currentSlide;
        QString Sstrokes = QString::fromStdString(strokes.toStdString());
        if (Sstrokes != "empty") {
            if (_server->_sh->checkForStrokes(QString::fromStdString(room->_roomID), room->_currentSlide)) {
                _server->_sh->saveStrokesInCache(QString::fromStdString(room->_roomID), room->_currentSlide, &Sstrokes);
                // std::cout << "changed Strokes for Slide #" << num << std::endl;
            }
            else{
                _server->_sh->saveStrokesInCache(QString::fromStdString(room->_roomID), room->_currentSlide, &Sstrokes);
                // std::cout << "set Strokes for Slide #" + QString::number(num).toStdString() << std::endl;
            }
        }
        if (num + 1 > room->_maxSlides && room->_sendSender) {
            _server->sendResponse(room->_response, "Slide", "success", "last", "", "");
            return;
        }
        /*std::vector<DatabaseRow> rows = _server->_presentations->getRowsByValue("ID", room->_presID);
        DatabaseRow row = rows[0];
        QString file = "Data/" + QString::fromStdString(row._fields[_server->_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num + 1) + ".jpg";*/

        QList<QVariant>* list = _server->_dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + QString::number(room->_presID) + "';");
        if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
            std::cerr << "DB-Error: SlideManager::nextSlide() " << list->at(1).toString().toStdString() << std::endl;
            return;
        }
        QString file = "Data/" + list->at(2).toString() + "/Slide" + QString::number(num + 1) + ".jpg";
        delete list;

        QByteArray b = _server->prepareSlide(file);
        if (QString::fromStdString(b.toStdString()) == "failed" && room->_sendSender){
            _server->sendResponse(room->_response, "Slide", "failed", QString::number(num), hasStrokes ? "true" : "false" , b);
            std::cout << "failed" << std::endl;
            return;
        }

        _server->sendResponse(room->_response, "Slide", "success", QString::number(num), hasStrokes ? "true" : "false", b);
        if (room->sendSlide(file, num)) {
            room->_currentSlide++;
        }
    }
}

void SlideManager::prevSlide(Room *room, QByteArray strokes, TcpServer *_server) {
    int num = room->_currentSlide;
    if (num - 1 < 1) {
        return;
    }
    QString Sstrokes = QString::fromStdString(strokes.toStdString());
    if (strokes != "empty") {
        if (_server->_sh->checkForStrokes(QString::fromStdString(room->_roomID), room->_currentSlide)) {
            _server->_sh->saveStrokesInCache(QString::fromStdString(room->_roomID), room->_currentSlide, &Sstrokes);
            // std::cout << "changed Strokes for Slide #" << num << std::endl;
        }
        else{
            _server->_sh->saveStrokesInCache(QString::fromStdString(room->_roomID), room->_currentSlide, &Sstrokes);
            // std::cout << "set Strokes for Slide #" + QString::number(num).toStdString() << std::endl;
        }
    }

    /*std::vector<DatabaseRow> rows = _server->_presentations->getRowsByValue("ID", room->_presID);
    if (rows.size() > 1)
        std::cerr << "Error in Database! There are 2 or more presentations with the same ID." << std::endl;
    DatabaseRow row = rows[0];

    QString file = "Data/" + QString::fromStdString(row._fields[_server->_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num - 1) + ".jpg";*/
    QList<QVariant>* list = _server->_dbm->singleSelect("SELECT folderName FROM presentations WHERE ID='" + QString::number(room->_presID) + "';");
    if (list->at(0).toBool() == false || list->at(1).toInt() != 1) {
        std::cerr << "DB-Error: SlideManager::nextSlide() " << list->at(1).toString().toStdString() << std::endl;
        return;
    }
    QString file = "Data/" + list->at(2).toString() + "/Slide" + QString::number(num - 1) + ".jpg";
    delete list;

    QByteArray b = _server->prepareSlide(file);
    if (QString::fromStdString(b.toStdString()) == "failed"){
        _server->sendResponse(room->_response, "Slide", "failed", QString::number(num), _server->_sh->checkForStrokes(QString::fromStdString(room->_roomID), num - 1) ? "true" : "false", b);
        std::cout << "failed" << std::endl;
        return;
    }

    _server->sendResponse(room->_response, "Slide", "success", QString::number(num), _server->_sh->checkForStrokes(QString::fromStdString(room->_roomID), num - 1) ? "true" : "false", b);
    if (room->sendSlide(file, num)) {
        room->_currentSlide--;
    }

}

