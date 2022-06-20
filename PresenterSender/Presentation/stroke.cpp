#include "stroke.h"

Stroke::Stroke(QObject *parent) : QObject(parent)
{

}

Stroke::Stroke(QColor c, int penWidth, QPolygon line) {
    _color = c;
    _penWidth = penWidth;
    _line = line;
}
