#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include <iostream>
#include <QSpacerItem>

UploadDialog::UploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);
    ui->BtnWeiter->setVisible(false);
    setCursor(Qt::WaitCursor);
}

UploadDialog::~UploadDialog()
{
    _list.clear();
    delete ui;
}

void UploadDialog::createList(QList<FileItem *> list)
{
    ui->scrollArea->takeWidget();
    QWidget* w = new QWidget(this);
    QVBoxLayout* v = new QVBoxLayout(w);

    foreach (FileItem* item, list) {
        QWidget* e = new QWidget(this);
        e->setLayout(item->getLayout());
        e->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        v->addWidget(e);
    }
    v->addSpacerItem(new QSpacerItem(40,20,QSizePolicy::Minimum, QSizePolicy::Expanding));
    w->setLayout(v);
    w->show();
    ui->scrollArea->setWidget(w);
    _list = list;
}

void UploadDialog::finished()
{
    setCursor(Qt::ArrowCursor);
    ui->BtnWeiter->setVisible(true);
    _finished = true;
    connect(ui->BtnWeiter, &QPushButton::clicked, this, &UploadDialog::close);
}

void UploadDialog::closeEvent(QCloseEvent *e)
{
    if (!_finished) {
        e->ignore();
    }
}

