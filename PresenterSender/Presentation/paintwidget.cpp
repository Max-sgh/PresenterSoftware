#include "paintwidget.h"
#include <QMouseEvent>
#include <QImage>
#include <QPainter>
#include <QStringBuilder>
#include <QLine>
#include <iostream>
#include <QPolygon>
#include <QFile>

PaintWidget::PaintWidget(QWidget* parent, Control* c)
{
    setAttribute(Qt::WA_StaticContents);
    _c = c;
    ur = new UndoRedo(this);
    ur->newStage(_strokes);
    _currentMode = Mode::draw;
    _penColor = Qt::black;
}

bool PaintWidget::openImage(const QString &fileName)
{
    QImage loaded_image;
    if (!loaded_image.load(fileName))
        return false;

    QSize newSize = loaded_image.size().expandedTo(size());
    resizeImage(&loaded_image, newSize);
    _image = loaded_image;
    _origImage = loaded_image;
    _modified = false;
    _currentMode = Mode::draw;
    update();
    return true;
}

void PaintWidget::endReached() {
    _image.fill(qRgb(255,255,255));
    QPainter painter(&_image);
    QFont f;
    f.setBold(true);
    f.setPointSize(20);
    painter.setFont(f);
    QFontMetrics fm(f);
    int width = fm.horizontalAdvance("Letzte Folie erreicht!");
    int height = fm.height();
    painter.drawText(QPoint(_image.size().width() / 2 - (width / 2), _image.size().height() /2 + (height / 2)), "Letzte Folie erreicht!");
    update();
}

void PaintWidget::saveCurrentImage()
{
    _currentSave = _image;
}

void PaintWidget::restoreCurrentImage()
{
    std::cout << "restore" << std::endl;
    QPainter painter(&_image);
    painter.setPen(QColor(0,0,0));
    painter.drawImage(QPoint(0,0), _currentSave);
    update();
}

void PaintWidget::fillWithColor(int r, int g, int b)
{
    _image.fill(qRgb(r, g, b));
    update();
}

void PaintWidget::setColor() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    std::cout << btn->objectName().toStdString() << std::endl;
    QColor c(btn->objectName());
    setPenColor(c);
}

void PaintWidget::changeMode(Mode newMode) {
    _currentMode = newMode;
}

bool PaintWidget::openImage(QImage loadedImage)
{
    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    _image = loadedImage;
    _modified = false;
    update();
    return true;
}

bool PaintWidget::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = _image;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        _modified = false;
        return true;
    }
    return false;
}

void PaintWidget::setPenColor(const QColor &newColor)
{
    _penColor = newColor;
}

void PaintWidget::setPenWidth(int newWidth)
{
    _penWidth = newWidth;
}

void PaintWidget::clearImage()
{
    _image.fill(qRgb(255, 255, 255));
    _modified = true;
    update();
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
{
    switch (_currentMode) {
        case Mode::draw:
            if (event->button() == Qt::LeftButton) {
                _lastPoint = event->pos();
                _points.push_back(_lastPoint);
                _isPainting = true;
            }
            break;
        case Mode::erase:
            if (event->button() == Qt::LeftButton) {
                _lastPoint = event->pos();
                _points.push_back(_lastPoint);
                _isPainting = true;
            }
            break;
        default:
            break;
    }
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    switch (_currentMode) {
        case Mode::draw:
            if ((event->buttons() & Qt::LeftButton) && _isPainting)
                drawLineTo(event->pos());
            break;
        case Mode::erase:
            //_points.push_back(event->pos());
            //_lastPoint = event->pos();
            int numChanged = 0;

            for (int i = 0; i < _strokes.size(); i++) {
                if (_strokes[i]->_line.containsPoint(QPoint(event->x(), event->y()), Qt::FillRule::WindingFill)) {
                    _strokes[i]->deleteLater();
                    _strokes.remove(i);
                    numChanged++;
                }
            }
            //_image.save("1.jpg");
            //_c->_net->_origImage.save("2.jpg");
            clearDraw();
            //_image.save("3.jpg");
            //_c->_net->_origImage.save("4.jpg");
            for (int i = 0; i < _strokes.size(); i++) {
                QPolygon p = _strokes[i]->_line;
                QPainter painter(&_image);
                painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap,
                                    Qt::RoundJoin));
                painter.drawPolyline(p);
                update();
            }
            if (numChanged > 0) {
                ur->newStage(_strokes);
                sendStrokes();
            }
            //QPixmap pix = QPixmap::fromImage(loadStrokes(getStrokeString()));
            //_c->_presScreen->setSlide(&pix);
            //std::cout << _strokes.length() << std::endl;
            break;
    }
}

void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
{
    switch (_currentMode) {
        case Mode::draw:
        if (event->button() == Qt::LeftButton && _isPainting) {
            drawLineTo(event->pos());
            QPolygon p(_points);
            //_lines.push_back(p);
            _strokes.push_back(new Stroke(_penColor, _penWidth, p));

            QString out;
            for (int i = 0; i < p.size(); i++) {
                out.append(QString::number(p[i].x()) + ":");
                out.append(QString::number(p[i].y()) + ":");
            }
            //_c->_net->sendMessage("stroke", QString::fromStdString(_c->_net->_id), "draw", QByteArray::fromStdString(out.toStdString()));
            sendStrokes();

            p.clear();

            _points.clear();
            _isPainting = false;
            ur->newStage(_strokes);
        }
            break;
        case Mode::erase:
            break;
        default:
            break;
    }
}

void PaintWidget::undo() {
    _strokes = ur->undo();
    clearDraw();
    for (int i = 0; i < _strokes.size(); i++) {
        QPolygon p = _strokes[i]->_line;
        QPainter painter(&_image);
        painter.setPen(QPen(_strokes[i]->_color, _strokes[i]->_penWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.drawPolyline(p);
        update();
    }
    sendStrokes();
}

void PaintWidget::redo() {
    _strokes = ur->redo();
    clearDraw();
    for (int i = 0; i < _strokes.size(); i++) {
        QPolygon p = _strokes[i]->_line;
        QPainter painter(&_image);
        painter.setPen(QPen(_strokes[i]->_color, _strokes[i]->_penWidth, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        painter.drawPolyline(p);
        update();
    }
    sendStrokes();
}

void PaintWidget::clearDraw() {
    clearImage();
    QSize newSize = _image.size().expandedTo(size());
    resizeImage(&_image, newSize);
    // _modified = false;
    QPainter p(&_image);
    p.drawImage(QPoint(0,0), _c->_net->_origImage);
    update();
}

void PaintWidget::sendStrokes() {
    QString out = getStrokeString();
    _c->_net->sendMessage("stroke", QString::fromStdString(_c->_net->_id), "draw", QByteArray::fromStdString(out.toStdString()));
}

QString PaintWidget::getStrokeString() {
    QString out;
    if (_strokes.size() == 0) {
        return "empty";
    }
    for (int i = 0; i < _strokes.size(); i++) {
        QPolygon p = _strokes[i]->_line;
        out.append("stroke:");
        out.append(_strokes[i]->_color.name() + ":");
        out.append(QString::number(_strokes[i]->_penWidth) + ":");
        for (int j = 0; j < p.size(); j++) {
            out.append(QString::number(p[j].x()) + ":");
            out.append(QString::number(p[j].y()) + ":");
        }
    }    return out;
}

void PaintWidget::clearStrokes() {
    _strokes.clear();
}

QImage PaintWidget::loadStrokes(QString data) {
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
        Stroke* s = new Stroke(QColor(hex), width, pol);
        _strokes.push_back(s);
        //_lines.push_back(pol);
        QPainter painter(&_image);
        // painter.drawImage(QPoint(0,0), _image);
        painter.setPen(QPen(s->_color, s->_penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPolyline(s->_line);
        points.clear();
    }
    return _image;

}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, _image, dirtyRect);
}

void PaintWidget::resizeEvent(QResizeEvent *event)
{
    if (width() > _image.width() || height() > _image.height()) {
        int newWidth = qMax(width() + 128, _image.width());
        int newHeight = qMax(height() + 128, _image.height());
        resizeImage(&_image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void PaintWidget::open(QString filename) {
    std::cout << "PaintWidget - open" << std::endl;
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        int i = 0;
        while (!f.atEnd()) {
            QVector<QPoint> points;
            QString line = f.readLine();
            if (line == "")
                break;
            line.remove("line:");
            line = line.left(line.lastIndexOf(":"));
            QStringList l = line.split(":");
            for (int i = 0; i < l.size(); i += 2) {
                QPoint p(l[i].toInt(), l[i+1].toInt());
                points.push_back(p);
            }
            QPolygon p(points);
            QPainter painter(&_image);
            painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap,
                                Qt::RoundJoin));
            painter.drawPolyline(p);
            _lines.push_back(p);
            points.clear();
            i++;
        }
    } else {
        std::cout << "Error" << f.errorString().toStdString() << std::endl;
    }
}

void PaintWidget::save(QString filename) {
    std::cout << "PaintWidget - save" << std::endl;
    QFile f(filename);
    if (f.open(QIODevice::ReadWrite)) {
        QString out;
        for (int i = 0; i < _lines.size(); i++) {
            out.append("line:");
            QPolygon pol = _lines[i];
            for (int i = 0; i < pol.size(); i++) {
                out.append(QString::number(pol[i].x()) + ":");
                out.append(QString::number(pol[i].y()) + ":");
            }
            out.append("\n");
        }
        f.write(out.toStdString().c_str(), out.toStdString().length());
    }
}

void PaintWidget::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&_image);
    painter.setPen(QPen(_penColor, _penWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(_lastPoint, endPoint);

    _points.push_back(endPoint);

    _modified = true;

    int rad = (_penWidth / 2) + 2;
    update(QRect(_lastPoint, endPoint).normalized()
                                     .adjusted(-rad, -rad, +rad, +rad));
    _lastPoint = endPoint;
}

void PaintWidget::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;

    QImage new_image(newSize, QImage::Format_RGB32);
    new_image.fill(qRgb(255, 255, 255));
    QPainter painter(&new_image);
    painter.drawImage(QPoint(0, 0), *image);
    *image = new_image;
}
