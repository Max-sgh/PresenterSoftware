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

class FileHandler : public QObject
{
    Q_OBJECT
public:
    explicit FileHandler(QObject *parent = nullptr);
    bool convertFile(Room* r, QString file, CDB* files, CDB* presentations);
    bool convertPDFWithEntry(Room* r, QString file, CDB* files, CDB* presentations);
    bool convertPDF(QString file, int* i, CDB* files);
    bool deleteFile(QString dirname, CDB* files, CDB* presentations);
    QImage renderSlide(QImage img, QString strokes);
    void exportToPDF(QVector<QImage> imgs, QString filename);

signals:

};

#endif // FILEHANDLER_H
