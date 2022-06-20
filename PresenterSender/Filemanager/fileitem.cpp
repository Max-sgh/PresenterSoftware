#include "fileitem.h"
#include <QLabel>
#include <QSpacerItem>
#include <QPixmap>
#include <iostream>


FileItem::FileItem(QObject *parent)
    : QObject{parent}
{

}
/*
 * QMovie* movie = new QMovie("ressources/loading.gif");
 * QLabel* gif = new QLabel("");
 * gif->setMovie(movie);
 * movie->start();
 */
QHBoxLayout *FileItem::getLayout()
{
    QHBoxLayout* ret = new QHBoxLayout(new QWidget());
    QLabel* name = new QLabel(_name);
    ret->addWidget(name);
    ret->addSpacerItem(new QSpacerItem(20,20,QSizePolicy::Expanding, QSizePolicy::Minimum));
    double kb = _size / 1024;
    double mb = kb / 1024;
    float value = (int)(mb * 100 + .5);
    mb = (float)value / 100;

    ret->addWidget(new QLabel(QString::number(mb) + "MB"));
    QLabel* icon = new QLabel("");
    switch (_status) {
        case Status::Loading:
            icon->setPixmap(QPixmap("ressources/state-sync.jpg"));
            break;
        case Status::Error:
            icon->setPixmap(QPixmap("ressources/state-error.jpg"));
            break;
        case Status::Success:
            icon->setPixmap(QPixmap("ressources/state-ok.jpg"));
            break;
        default:
            icon->setPixmap(QPixmap("ressources/state-offline.jpg"));
            break;
    }
    ret->addWidget(icon);
    return ret;
}
