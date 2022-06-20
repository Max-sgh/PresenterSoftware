#ifndef PRESENTATIONSCREEN_H
#define PRESENTATIONSCREEN_H

#include <QMainWindow>
#include <QLabel>
#include <iostream>
#include <QPushButton>
#include "../control.h"
#include <QFrame>
#include <QSlider>
#include <QGroupBox>

namespace Ui {
class PresentationScreen;
}

class PresentationScreen : public QMainWindow
{
    Q_OBJECT

public:
    // PresentationScreen(QWidget *parent = nullptr);
    PresentationScreen(Control* c);
    ~PresentationScreen();
    QWidget* _w;
    void btnNextClicked();
    void btnPrevClicked();
    void btnBlackClicked();
    void btnWhiteClicked();
    void btnDrawClicked();
    void setSlide(QPixmap* pix);

public slots:
    void actionDraw();
    void actionErase();
    void actionUndo();
    void actionRedo();
    void optionButtonPressed();

private:
    Ui::PresentationScreen *ui;
    QPushButton* _btnNext;
    QPushButton* _btnPrev;
    QPushButton* _btnBlack;
    QPushButton* _btnWhite;
    QGroupBox* _group;
    // QPushButton* _btnDraw;
    QLabel* _imageShow;
    Control* _c;
};

#endif // PRESENTATIONSCREEN_H
