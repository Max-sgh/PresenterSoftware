#include "presentationscreen.h"
#include "ui_presentationscreen.h"
#include <QGridLayout>
#include <QPixmap>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>

PresentationScreen::PresentationScreen(Control* c) :
    // QMainWindow(parent),
    ui(new Ui::PresentationScreen)
{
    _c = c;
    _c->_pw = new PaintWidget(this, _c);
    ui->setupUi(this);
    delete ui;

    _w = new QWidget();

    _imageShow = new QLabel("Waiting for Data");
    _btnNext = new QPushButton("Next");
    _btnPrev = new QPushButton("Prev");
    _btnBlack = new QPushButton("Black");
    _btnWhite = new QPushButton("White");
    _group = new QGroupBox("Annotationseinstellungen");
    // _btnDraw = new QPushButton("Draw");

    QGridLayout* grid = new QGridLayout(this);
    grid->addWidget(_c->_pw, 0, 1, 2, 6, Qt::AlignCenter);
    _c->_pw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //grid->addWidget(_imageShow, 0, 0, 2, 5, Qt::AlignCenter);
    //_imageShow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    QPixmap pix("test.jpg");
//    _imageShow->setPixmap(pix);

    grid->addWidget(_btnWhite, 2, 1, Qt::AlignCenter);
    _btnWhite->setFixedSize(QSize(150,100));
    connect(_btnWhite, &QPushButton::clicked, this, &PresentationScreen::btnWhiteClicked);

    grid->addWidget(_btnBlack, 2, 2, Qt::AlignCenter);
    _btnBlack->setFixedSize(QSize(150,100));
    connect(_btnBlack, &QPushButton::clicked, this, &PresentationScreen::btnBlackClicked);

    grid->addWidget(_btnPrev, 2, 3, Qt::AlignCenter);
    _btnPrev->setFixedSize(QSize(150,100));
    connect(_btnPrev, &QPushButton::clicked, this, &PresentationScreen::btnPrevClicked);

    grid->addWidget(_btnNext, 2, 4, Qt::AlignCenter);
    _btnNext->setFixedSize(QSize(150,100));
    connect(_btnNext, &QPushButton::clicked, this, &PresentationScreen::btnNextClicked);

    QGridLayout* grid2 = new QGridLayout(this);
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setValue(2);
    connect(slider, &QSlider::valueChanged, _c->_pw, &PaintWidget::setPenWidth);
    grid2->addWidget(slider, 0, 0, 1, 2, Qt::AlignCenter);

    QSize s(80,35);
    QPushButton* btnDraw = new QPushButton("Draw");
    connect(btnDraw, &QPushButton::clicked, this, &PresentationScreen::actionDraw);
    btnDraw->setFixedSize(s);
    grid2->addWidget(btnDraw, 1, 0, Qt::AlignCenter);

    QPushButton* btnErase = new QPushButton("Erase");
    btnErase->setFixedSize(s);
    connect(btnErase, &QPushButton::clicked, this, &PresentationScreen::actionErase);
    grid2->addWidget(btnErase, 1, 1, Qt::AlignCenter);

    QPushButton* btnColorRed = new QPushButton(this);
    btnColorRed->setAutoFillBackground(true);
    QPalette pal = btnColorRed->palette();
    pal.setColor(QPalette::Button, QColor(Qt::red));
    btnColorRed->setPalette(pal);
    btnColorRed->update();
    btnColorRed->setFixedSize(s);
    btnColorRed->setObjectName("red");
    connect(btnColorRed, &QPushButton::clicked, _c->_pw, &PaintWidget::setColor);
    grid2->addWidget(btnColorRed, 2, 0, Qt::AlignCenter);

    QPushButton* btnColorBlue = new QPushButton(this);
    btnColorBlue->setAutoFillBackground(true);
    pal = btnColorBlue->palette();
    pal.setColor(QPalette::Button, QColor(Qt::blue));
    btnColorBlue->setPalette(pal);
    btnColorBlue->update();
    btnColorBlue->setFixedSize(s);
    btnColorBlue->setObjectName("blue");
    connect(btnColorBlue, &QPushButton::clicked, _c->_pw, &PaintWidget::setColor);
    grid2->addWidget(btnColorBlue, 2, 1, Qt::AlignCenter);

    QPushButton* btnColorGreen = new QPushButton(this);
    btnColorGreen->setAutoFillBackground(true);
    pal = btnColorGreen->palette();
    pal.setColor(QPalette::Button, QColor(Qt::green));
    btnColorGreen->setPalette(pal);
    btnColorGreen->update();
    btnColorGreen->setFixedSize(s);
    btnColorGreen->setObjectName("green");
    connect(btnColorGreen, &QPushButton::clicked, _c->_pw, &PaintWidget::setColor);
    grid2->addWidget(btnColorGreen, 3, 0, Qt::AlignCenter);

    _group->setLayout(grid2);

    grid->addWidget(_group, 2, 5, Qt::AlignCenter);

    // QSpacerItem* item = new QSpacerItem(200,20,QSizePolicy::Minimum, QSizePolicy::Expanding);
    // grid->addItem(item,0,5,2,1);

    QVBoxLayout* layout = new QVBoxLayout();
    QPushButton* btn1 = new QPushButton("Präsentation beenden");
    btn1->setObjectName("close");
    connect(btn1, &QPushButton::clicked, this, &PresentationScreen::optionButtonPressed);
    QPushButton* btn2 = new QPushButton("Screensharing aktivieren");
    btn2->setObjectName("screensharing");
    connect(btn2, &QPushButton::clicked, this, &PresentationScreen::optionButtonPressed);
    QPushButton* btn3 = new QPushButton("Präsentation ändern");
    btn3->setObjectName("change");
    connect(btn3, &QPushButton::clicked, this, &PresentationScreen::optionButtonPressed);
    QSpacerItem* item0 = new QSpacerItem(40,20,QSizePolicy::Minimum, QSizePolicy::Expanding);
    layout->addWidget(btn1);
    layout->addWidget(btn2);
    layout->addWidget(btn3);
    layout->addSpacerItem(item0);

    QGroupBox* f = new QGroupBox("Optionen");
    f->setLayout(layout);
    grid->addWidget(f, 2, 6, Qt::AlignCenter);

    QSpacerItem* item = new QSpacerItem(200,20,QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(item,0,0,3,1);
    QSpacerItem* item1 = new QSpacerItem(200,20,QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(item1,0,7,3,1);
    QSpacerItem* item2 = new QSpacerItem(200,20,QSizePolicy::Expanding, QSizePolicy::Expanding);
    grid->addItem(item2,3,0,1,7);

    _w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _w->setLayout(grid);
}

void PresentationScreen::optionButtonPressed() {
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (btn->objectName() == "close") {
        _c->_pres->close();
    }
    else if (btn->objectName() == "screensharing") {
        _c->_net->sendMessage("prepareScreenshare", QString::fromStdString(_c->_net->_id), "", "");
    }
}

void PresentationScreen::actionDraw() {
    _c->_pw->changeMode(PaintWidget::Mode::draw);
}

void PresentationScreen::actionErase() {
    _c->_pw->changeMode(PaintWidget::Mode::erase);
}

void PresentationScreen::actionUndo() {
    _c->_pw->undo();
}

void PresentationScreen::actionRedo() {
    _c->_pw->redo();
}

void PresentationScreen::btnNextClicked() {
    _c->_net->sendMessage("nextSlide", QString::fromStdString(_c->_net->_id), "", QByteArray::fromStdString(_c->_pw->getStrokeString().toStdString()));
    _c->_pw->clearStrokes();
}

void PresentationScreen::btnBlackClicked() {
    //std::cout << "Black" << std::endl;
    _c->_net->sendMessage("black", QString::fromStdString(_c->_net->_id), "", "");
}

/*void Presentation::btnDrawClicked() {
    std::cout << "Draw" << std::endl;
    // _c->_net->sendMessage("closeRoom", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString("null"));
}*/

void PresentationScreen::btnPrevClicked() {
    _c->_net->sendMessage("prevSlide", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString(_c->_pw->getStrokeString().toStdString()));
    _c->_pw->clearStrokes();
}

void PresentationScreen::btnWhiteClicked() {
    //std::cout << "White" << std::endl;
    _c->_net->sendMessage("white", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString(""));
}

void PresentationScreen::setSlide(QPixmap *pix) {
    //_imageShow->setPixmap(*pix);
    _c->_pw->openImage(pix->toImage());
    _c->_pw->setMinimumHeight(pix->toImage().size().height());
    _c->_pw->setMinimumWidth(pix->toImage().size().width());
    _c->_pw->_origImage = pix->toImage();
}

PresentationScreen::~PresentationScreen()
{
    // delete ui;
}
