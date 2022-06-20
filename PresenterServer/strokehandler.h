#ifndef STROKEHANDLER_H
#define STROKEHANDLER_H

#include <QObject>
#include "room.h"

class Strokehandler : public QObject
{
    Q_OBJECT
public:
    explicit Strokehandler(QObject *parent = nullptr);
    bool checkForStrokes(QString roomID, int slideNum);
    void saveStrokesInCache(QString roomID, int slideNum, QString* strokes);
    QString getStrokes(QString roomID, int slideNum);
    void saveStrokes(Room* r, QString folderName);
    void openStrokes(Room* r, QString folderName);
    void cleanCache(Room* r, QString folderName);

signals:

};

#endif // STROKEHANDLER_H
