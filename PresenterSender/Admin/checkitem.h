#ifndef CHECKITEM_H
#define CHECKITEM_H

#include <QObject>

class CheckItem : public QObject
{
    Q_OBJECT
public:
    explicit CheckItem(QString name, QString action, QObject *parent = nullptr) {
        _action = action;
        _name = name;
        _status = Status::Loading;
    }
    enum Status { Loading, Success, Error};
    Status _status;
    QString _name;
    QString _action;
};

#endif // CHECKITEM_H
