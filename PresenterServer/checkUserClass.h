#ifndef CHECKUSERCLASS_H
#define CHECKUSERCLASS_H

#include <QObject>

class CheckUser : public QObject
{
    Q_OBJECT
public:
    explicit CheckUser(QString username, QString firstname, QString lastname, int id, QObject *parent = nullptr) {
        _username = username;
        _firstname = firstname;
        _lastname = lastname;
        _id = id;
    }
    QString _username;
    QString _firstname;
    QString _lastname;
    int _id;

};

#endif // CHECKUSERCLASS_H
