#ifndef FILEITEM_H
#define FILEITEM_H

#include <QObject>
#include <QHBoxLayout>

class FileItem : public QObject
{
    Q_OBJECT
public:
    explicit FileItem(QObject *parent = nullptr);

    enum Status { Loading, Success, Error};

    int _id;
    QString _name;
    double _size;
    Status _status;


    QHBoxLayout* getLayout();

};

#endif // FILEITEM_H
