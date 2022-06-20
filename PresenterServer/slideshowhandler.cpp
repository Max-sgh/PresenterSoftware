#include "slideshowhandler.h"
#include <iostream>

SlideshowHandler::SlideshowHandler(QObject *parent, TcpServer* server)
    : QObject{parent}
{
    _server = server;
}

// Überladung mit QString als return, für Room::sendSlide(QString)

QByteArray SlideshowHandler::nextSlide(QString room, QString* ret)
{
    /*Room* r = _server->getRoom(room);
    if (r->_currentSlide == -1) {
        std::vector<DatabaseRow> rows = _server->_presentations->getRowsByValue("name", r->_presentation);
        DatabaseRow row = rows[0];
        int num = 1;

        QString file = "Data/" + QString::fromStdString(row._fields[_server->_presentations->getIndexByName("folderName")].s) + "/Slide1.jpg";
        QByteArray b = _server->prepareSlide(file);
        r->_currentSlide = 1;
        return b;
    }
    else {
        int num = r->_currentSlide;
        if (num + 1 > r->_maxSlides) {
            //sendResponse(socket, "Slide", "success", "last", "", "");
            return "last";
        }
        std::vector<DatabaseRow> rows = _server->_presentations->getRowsByValue("name", r->_presentation);
        DatabaseRow row = rows[0];
        QString file = "Data/" + QString::fromStdString(row._fields[_server->_presentations->getIndexByName("folderName")].s) + "/Slide" + QString::number(num + 1) + ".jpg";

        QByteArray b = _server->prepareSlide(file);
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
