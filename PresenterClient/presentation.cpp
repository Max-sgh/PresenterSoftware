#include "presentation.h"
#include "ui_presentation.h"
#include <QFile>
#include <QPainter>
#include <iostream>
#include <QColor>

Presentation::Presentation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Presentation)
{
    ui->setupUi(this);
    //QPixmap pix("test.jpg");
    ui->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setText("Waiting for Data...");
    // _currentSlide = new QPixmap("test.jpg");
    //setSlide(new QPixmap("test.jpg"));
}

void Presentation::open(QString filename) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly)) {
        int i = 0;
        while (!f.atEnd()) {
            std::cout << "line" << std::endl;
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
            QImage img = _currentSlide->toImage();
            QPainter painter(&img);
            painter.setPen(QPen(QColor::fromRgb(0,0,0), 2, Qt::SolidLine, Qt::RoundCap,
                                Qt::RoundJoin));
            painter.drawPolyline(p);
            /*QPixmap* pix = new QPixmap();
            pix->fromImage(img);
            pix->save("n" + QString::number(i) + ".jpg");
            pix->save("q.jpg");
            setSlide(pix);*/
            //_lines.push_back(p);
            points.clear();
            i++;
        }
    } else {
        std::cout << "Error" << f.errorString().toStdString() << std::endl;
    }
}

void Presentation::setBlack () {
    /*std::cout << "black" << std::endl;
    //*_currentSave = _currentSlide->toImage();
    std::cout << "1" << std::endl;
    // _currentSlide->fill(qRgb(0,0,0));
    ui->label->setPixmap(*_currentSlide);
    // _image->fill(QColor(0,0,0));
    std::cout << "2" << std::endl;
    update();
    std::cout << "black" << std::endl;*/
    QPixmap pix(_size);
    //pix = *_currentSlide;
    std::cout << pix.size().width() << "" << pix.size().height() << std::endl;
    pix.fill(QColor::fromRgb(0,0,0));
    ui->label->setPixmap(pix);
    //ui->label->setStyleSheet("QLabel { background-color : black; }");
    /*
    std::cout << 1 << std::endl;
    _origImage->fill(qRgb(255,255,255));
    _currentSlide->fill(qRgb(255,255,255));
    std::cout << 1 << std::endl;
    update();
    std::cout << 1 << std::endl;*/
}

void Presentation::restoreSlide() {
    /*QPainter painter(_image);
    painter.setPen(QColor(255,255,255));
    painter.drawImage(QPoint(0,0), *_currentSave);
    update();*/
    std::cout << "restore" << std::endl;
    //_currentSlide = _pixSave;
    //ui->label->setPixmap(*_currentSlide);
}

void Presentation::clearDraw() {
    _currentSlide->fill(qRgb(255,255,255));
    //QPainter p(_currentSlide);
    //p.drawImage(QPoint(0,0), _origImage);
    update();
}

void Presentation::setSlide(QPixmap* p) {
    //QPixmap pix = p.copy();
    //_currentSlide = p;
    /*QPixmap pix2 = pix.copy();
    _origImage = &pix2;*/
    // ui->label->clear();
    _size = p->size();
    ui->label->setPixmap(/*p->scaled(size().width()-20, size().height()-20, Qt::KeepAspectRatio)*/ *p);
}

Presentation::~Presentation()
{
    delete ui;
}
