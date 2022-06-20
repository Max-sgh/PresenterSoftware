#ifndef STROKE_H
#define STROKE_H

#include <QObject>
#include <QColor>
#include <QPolygon>

class Stroke : public QObject
{
    Q_OBJECT
public:
    explicit Stroke(QObject *parent = nullptr);
    Stroke(QColor c, int penWidth, QPolygon line);
    QColor _color;
    QPolygon _line;
    int _penWidth;

signals:

};

#endif // STROKE_H
