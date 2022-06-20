#include "loadinganimation.h"
#include <QLabel>
#include <iostream>
#include <QPainter>

loadingAnimation::loadingAnimation(QString file, QWidget *parent)
    : QLabel{""}
{
    _pixmap = new QPixmap(file);
    _timer = new QTimer();
    _timer->setInterval(_interval);
    connect(_timer, &QTimer::timeout, this, &loadingAnimation::tick);
    this->setPixmap(*_pixmap);
    _height = _pixmap->height();
    _width = _pixmap->width();
}

loadingAnimation::loadingAnimation(QPixmap* pix, QWidget *parent)
    : QLabel{""}
{
    _pixmap = pix;
}


loadingAnimation::~loadingAnimation()
{
    delete _pixmap;
    delete _timer;
}

void loadingAnimation::startAnimation()
{
    _timer->start();
}

void loadingAnimation::stopAnimation()
{
    _timer->stop();
}

void loadingAnimation::tick()
{
    i++;
    /*if (i > 5) {
        _timer->stop();
    }*/
    QPainter painter(_pixmap);
    painter.rotate(90);
    painter.drawPixmap(QPoint(0,0), *_pixmap);
    update();

    /*QTransform tr;
    tr.rotate(30);
    QPixmap pix = _pixmap->transformed(tr);
    pix = pix.scaled(_width, _height);
    this->setPixmap(pix);

    delete _pixmap;
    _pixmap = new QPixmap(pix);*/
    // std::cout << pix.width() << " " << pix.height() << std::endl;

}
