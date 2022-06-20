#include "strokehandler.h"
#include "Database/CDB.h"
#include "Database/CDBRow.h"
#include "Database/CDBFileHandler.h"
#include <iostream>
#include <QFile>


Strokehandler::Strokehandler(QObject *parent) : QObject(parent)
{

}

void Strokehandler::saveStrokes(Room* r, QString folderName) {
    for (int i = 0; i < r->_maxSlides; i++) {
        if (checkForStrokes(QString::fromStdString(r->_roomID), i)) {
            QString dest = "Data/" + folderName + "/Slide" + QString::number(i) + ".strokes";
            QString source = "Data/Cache/" + QString::fromStdString(r->_roomID) + "-" + QString::number(i) + ".strokes";
            QFile destF(dest);
            QFile sourceF(source);
            if (destF.open(QIODevice::WriteOnly)) {
                if (sourceF.open(QIODevice::ReadOnly)) {
                    destF.write(sourceF.readAll());
                    sourceF.remove();
                    sourceF.close();
                    destF.close();
                }
            }
        }
    }
}

void Strokehandler::openStrokes(Room* r, QString folderName) {
    for (int i = 0; i < r->_maxSlides; i++) {
        if (QFile::exists("Data/" + folderName + "/Slide" + QString::number(i) + ".strokes")) {
            QString source = "Data/" + folderName + "/Slide" + QString::number(i) + ".strokes";
            QString dest = "Data/Cache/" + QString::fromStdString(r->_roomID) + "-" + QString::number(i) + ".strokes";
            QFile destF(dest);
            QFile sourceF(source);
            if (destF.open(QIODevice::WriteOnly)) {
                if (sourceF.open(QIODevice::ReadOnly)) {
                    destF.write(sourceF.readAll());
                    sourceF.close();
                    destF.close();
                }
            }
        }
    }

}

void Strokehandler::cleanCache(Room* r, QString folderName) {
    for (int i = 0; i < r->_maxSlides; i++) {
        if (checkForStrokes(QString::fromStdString(r->_roomID), i)) {
            QString source = "Data/Cache/" + QString::fromStdString(r->_roomID) + "-" + QString::number(i) + ".strokes";
            QFile sourceF(source);
            if (sourceF.open(QIODevice::ReadOnly)) {
                sourceF.remove();
                sourceF.close();
            }
        }
    }
}

bool Strokehandler::checkForStrokes(QString roomID, int slideNum) {
    QString file = "Data/Cache/" + roomID + "-" + QString::number(slideNum) + ".strokes";
    QFile f(file);
    return f.exists();
}

void Strokehandler::saveStrokesInCache(QString roomID, int slideNum, QString* strokes) {
    QString file = "Data/Cache/" + roomID + "-" + QString::number(slideNum) + ".strokes";
    QFile f(file);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(strokes->toStdString().c_str());
        f.close();
    } else {
        std::cerr << "Can´t open the File - " << file.toStdString() << std::endl;
    }
}

QString Strokehandler::getStrokes(QString roomID, int slideNum) {
    QString ret;
    QString file = "Data/Cache/" + roomID + "-" + QString::number(slideNum) + ".strokes";
    QFile f(file);
    if (f.open(QIODevice::ReadOnly)) {
        ret = f.readAll();
        f.close();
    } else {
        std::cerr << "Can´t open the File - " << file.toStdString() << std::endl;
    }
    return ret;
}
