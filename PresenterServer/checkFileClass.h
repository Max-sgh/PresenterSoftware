#ifndef CHECKFILECLASS_H
#define CHECKFILECLASS_H

#include <QObject>

class CheckFile : public QObject
{
    Q_OBJECT
public:
    explicit CheckFile(QString user, QString foldername, QString name, int id, QString type, QObject *parent = nullptr) {
        _user = user;
        _foldername = foldername;
        _name = name;
        _type = type;
        _id = id;
    }
    QString _user;
    QString _foldername;
    QString _name;
    QString _type;
    int _id;

};

#endif // CHECKFILECLASS_H
