#ifndef CHECKPRESCLASS_H
#define CHECKPRESCLASS_H

#include <QObject>

class CheckPres : public QObject
{
    Q_OBJECT
public:
    explicit CheckPres(QString user, QString foldername, QString name, int id, int numSlides, QObject *parent = nullptr) {
        _user = user;
        _foldername = foldername;
        _name = name;
        _numSlides = numSlides;
        _id = id;
    }
    QString _user;
    QString _foldername;
    QString _name;
    int _numSlides;
    int _id;

};

#endif // CHECKPRESCLASS_H
