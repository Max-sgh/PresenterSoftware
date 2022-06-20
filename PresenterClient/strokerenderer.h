#ifndef STROKERENDERER_H
#define STROKERENDERER_H

#include <QObject>
#include <QImage>

class StrokeRenderer : public QObject
{
    Q_OBJECT
public:
    StrokeRenderer(QObject *parent, QString filename);
    StrokeRenderer(QObject *parent = nullptr);
    QImage render(QString data);
    void loadImage();
    void loadImage(QImage loadedImg);
    QImage _image;

public slots:
    void clearImage();

private:
    void resizeImage(QImage *image, const QSize &newSize);
    void clearDraw();
    QString _filename;
    QImage _origImage;


};


#endif // STROKERENDERER_H
