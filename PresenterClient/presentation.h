#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <QDialog>
#include <QLabel>
#include <QPixmap>

namespace Ui {
class Presentation;
}

class Presentation : public QDialog
{
    Q_OBJECT

public:
    explicit Presentation(QWidget *parent = nullptr);
    ~Presentation();
    void open(QString filename);
    void clearDraw();

public slots:
    void setSlide(QPixmap* p);
    void setBlack();
    void restoreSlide();

private:
    Ui::Presentation *ui;
    QPixmap* _currentSlide;
    QPixmap* _origImage;
    QImage* _image;
    QSize _size;
};

#endif // PRESENTATION_H
