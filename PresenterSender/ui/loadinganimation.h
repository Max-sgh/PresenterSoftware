#ifndef LOADINGANIMATION_H
#define LOADINGANIMATION_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

class loadingAnimation : public QLabel
{
    Q_OBJECT
public:
    loadingAnimation(QString file, QWidget *parent = nullptr);
    loadingAnimation(QPixmap* file, QWidget *parent = nullptr);

    void setInterval(int interval) { _interval = interval; }

    ~loadingAnimation();

public slots:
    void startAnimation();
    void stopAnimation();

private:
    QPixmap* _pixmap;
    QTimer* _timer;
    int _interval = 200;
    void tick();
    int i = 0;
    int _width;
    int _height;

};

#endif // LOADINGANIMATION_H
