#include "strokerenderer.h"
#include <QFile>
#include <QPoint>
#include <QPolygon>
#include <QImage>
#include <QPainter>
#include <iostream>

StrokeRenderer::StrokeRenderer(QObject *parent, QString filename) : QObject(parent)
{
    _filename = filename;
}

StrokeRenderer::StrokeRenderer(QObject *parent) {

}

void StrokeRenderer::loadImage(QImage loadedImg) {
    QSize newSize = loadedImg.size();
    resizeImage(&loadedImg, newSize);
    _image = loadedImg;
    _origImage = _image;
}


void StrokeRenderer::loadImage() {
    QImage loadedImg;
    if (!loadedImg.load(_filename))
        return;
    QSize newSize = loadedImg.size();
    resizeImage(&loadedImg, newSize);
    _image = loadedImg;
    _origImage = _image;
}

void StrokeRenderer::resizeImage(QImage *image, const QSize &newSize) {
    if (image->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

QImage StrokeRenderer::render(QString data)
{
    QImage img = _image;
    QStringList l = data.split("stroke:");
    l.removeAt(0);

    for (int i = 0; i < l.size(); i++) {
        QString line = l[i];
        line = line.left(line.lastIndexOf(":"));
        QVector<QPoint> points;
        QStringList l = line.split(":");

        QString hex = l[0];
        l.removeAt(0);
        int width = l[0].toInt();
        l.removeAt(0);

        for (int j = 0; j < l.size(); j += 2) {
            QPoint p(l[j].toInt(), l[j+1].toInt());
            points.push_back(p);
        }

        QPolygon pol(points);
        QPainter painter(&img);
        painter.drawImage(QPoint(0,0), img);
        painter.setPen(QPen(QColor(hex), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPolyline(pol);
    }
    _image = img;
    return _image;
}

void StrokeRenderer::clearDraw() {
    clearImage();
    QSize newSize = _image.size();
    resizeImage(&_image, newSize);
    QPainter p(&_image);
    p.drawImage(QPoint(0,0), _origImage);
}

void StrokeRenderer::clearImage() {
    _image.fill(qRgb(255,255,255));
}
