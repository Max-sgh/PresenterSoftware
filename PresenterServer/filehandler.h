#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QImage>
#include "room.h"
#include "Database/CDB.h"
#include "Database/CDBFileHandler.h"
#include "Database/CDBRow.h"
#include "dbmanager.h"

class FileHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileHandler(QObject *parent = nullptr);
    bool convertFile(Room* r, QString file, DbManager* dbm);
    bool convertPDFWithEntry(Room* r, QString file, DbManager* dbm);
    bool convertPDF(QString file, int* i, DbManager* dbm);
    bool deleteFile(QString dirname, DbManager* dbm);
    QImage renderSlide(QImage img, QString strokes);
    void exportToPDF(QVector<QImage> imgs, QString filename);

signals:

};

#endif // FILEHANDLER_H
