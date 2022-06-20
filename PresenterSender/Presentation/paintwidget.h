#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QVector>
#include <QWidget>
#include "../control.h"
#include "undoredo.h"
#include "stroke.h"

class PaintWidget : public QWidget
{
    Q_OBJECT
public:
    enum Mode : int { draw = 1, erase = 2 };
    explicit PaintWidget(QWidget *parent = nullptr, Control* c = nullptr);

    bool openImage(const QString &fileName);
    bool openImage(QImage loadedImage);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void save(QString filename);
    void open(QString filename);

    bool isModified() const { return _modified; }
    QColor penColor() const { return _penColor; }
    int penWidth() const { return _penWidth; }

    void undo();
    void redo();
    void changeMode(Mode newMode);

    void sendStrokes();
    QImage _origImage;
    QString getStrokeString();
    void clearStrokes();
    QImage loadStrokes(QString data);
    void saveCurrentImage();
    void restoreCurrentImage();
    void fillWithColor(int r, int g, int b);

public slots:
    void clearImage();
    void setPenWidth(int newWidth);
    void setColor();
    void endReached();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLineTo(const QPoint &endPoint);
    void resizeImage(QImage *image, const QSize &newSize);
    void clearDraw();

    bool _modified = false;
    bool _isPainting = false;
    int _penWidth = 2;
    QColor _penColor = Qt::blue;
    QImage _image;
    QPoint _lastPoint;
    QVector<QPolygon> _lines;
    QVector<Stroke*> _strokes;
    QVector<QPoint> _points;
    Control* _c;
    Mode _currentMode;
    UndoRedo* ur;
    QImage _currentSave;

};

#endif // PAINTWIDGET_H
