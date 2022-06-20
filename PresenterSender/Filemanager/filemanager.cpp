#include "filemanager.h"
#include "ui_filemanager.h"
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QMovie>
#include "ui/loadinganimation.h"
#include "fileitem.h"

FileManager::FileManager(QWidget *parent, Control* c) :
    QMainWindow(parent),
    ui(new Ui::FileManager)
{
    ui->setupUi(this);
    _c = c;
    _blockButtons = false;
    connect(ui->BtnBack, &QPushButton::clicked, this, &FileManager::BtnBackClicked);
    this->setWindowTitle("Dateimanager");
}

void FileManager::createFileList()
{
    ui->scrollArea->takeWidget();
    _blockButtons = false;
    _isFileDialog = false;
    QStringList list = _fileList.split(";");
    QWidget* w = new QWidget(this);
    QVBoxLayout* v = new QVBoxLayout(this);
    for (int i = 0; i < list.size(); i++) {
        QString mimetype = list[i].split("->")[1];
        QString filename = list[i].split("->")[0];

        QStringList presMime = { "ppt", "pptx", "odp" };
        QPixmap* img;
        if (presMime.contains(mimetype)) {
            img = new QPixmap("ressources/mimetype-presentation.jpg");
        }
        else if (mimetype == "pdf") {
            img = new QPixmap("ressources/mimetype-pdf.jpg");
        }
        else {
            img = new QPixmap("ressources/mimetype-unknown.jpg");
        }
        QLabel* imgShow = new QLabel("");
        imgShow->setPixmap(img->scaled(32,32, Qt::KeepAspectRatio));

        QHBoxLayout* h = new QHBoxLayout(this);
        QLabel* l = new QLabel(filename + "." + mimetype);
        QSpacerItem* spacer = new QSpacerItem(40,20, QSizePolicy::Expanding, QSizePolicy::Minimum);
        /*QPushButton* p1 = new QPushButton("B");
        p1->setObjectName(QString::number(i) + "-B");*/
        QPixmap trash("ressources/trash.png");
        QIcon i1(trash);
        QPixmap pdf("ressources/PDF.png");
        QIcon i2(pdf);

        QPushButton* p2 = new QPushButton("");
        p2->setIcon(i2);
        p2->setObjectName(QString::number(i) + "-P");

        QPushButton* p3 = new QPushButton("");
        p3->setObjectName(QString::number(i) + "-L");
        p3->setIcon(i1);

        //connect(p1, &QPushButton::clicked, this, &FileManager::ButtonClicked);
        connect(p2, &QPushButton::clicked, this, &FileManager::ButtonClicked);
        connect(p3, &QPushButton::clicked, this, &FileManager::ButtonClicked);

        h->addWidget(imgShow);
        h->addWidget(l);
        h->addSpacerItem(spacer);
        //h->addWidget(p1);
        h->addWidget(p2);
        h->addWidget(p3);
        v->addLayout(h);
    }
    QPushButton* addButton = new QPushButton("+");
    v->addWidget(addButton);
    connect(addButton, &QPushButton::clicked, this, &FileManager::addButtonClicked);
    QSpacerItem* sp2 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    v->addSpacerItem(sp2);
    w->setLayout(v);
    ui->scrollArea->setWidget(w);
    w->show();
    setCursor(Qt::ArrowCursor);
}

void FileManager::ButtonClicked() {
    if (_blockButtons) {
        return;
    }
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    QStringList list = _fileList.split(";");
    int num = btn->objectName().split("-")[0].toInt();
    QString operation = btn->objectName().split("-")[1];
    std::cout << operation.toStdString() << std::endl;

    int id = list[num].split("->")[2].toInt();
    QString name = list[num].split("->")[0];
    QString extension = list[num].split("->")[1];

    if (operation == "L") {
        setCursor(Qt::WaitCursor);
        _c->_net->sendMessage("deleteFile", QString::fromStdString(_c->_net->_id), QString::number(id), "");
        _blockButtons = true;
    } else if (operation == "P") {
        setCursor(Qt::WaitCursor);
        _c->_net->sendMessage("exportToPDF", QString::fromStdString(_c->_net->_id), QString::number(id), "");
        _blockButtons = true;
    }
}

void FileManager::addButtonClicked() {
    if (_blockButtons) {
        return;
    }
    _isFileDialog = true;
    QStringList _filenames = QFileDialog::getOpenFileNames(this, "Dateien hochladen", QDir::homePath() , tr("Dokumente (*.pptx *.ppt *.odp *.doxc *.doc *.odt *.pdf)"));
    if (_filenames.empty()) {
        return;
    }
    _uploadedFiles = _filenames.size();
    setCursor(Qt::WaitCursor);
    // ui->statusbar->showMessage("Konvertiere Datei(-en)...");
    int i = 1;
    _dialog = new UploadDialog(this);
    QList<FileItem*> list;
    foreach (QString s, _filenames) {
        QFileInfo fileInfo(s);
        QFile f(s);
        if (f.open(QIODevice::ReadOnly) && f.isReadable()) {
            FileItem* item = new FileItem(_dialog);
            item->_id = i;
            item->_name = fileInfo.fileName();
            item->_size = fileInfo.size();
            item->_status = FileItem::Status::Loading;
            list << item;

            _c->_net->sendMessage("uploadFile", QString::fromStdString(_c->_net->_id) + "$$" + QString::number(i), fileInfo.fileName(), f.readAll());

            f.close();
            i++;
        }
        else
            std::cerr << "Error! - " << f.errorString().toStdString() << std::endl;
    }
    _dialog->createList(list);
    _dialog->show();
    connect(_c->_net, &Network::changeFileStatus, this, &FileManager::changeFileStatus);
    _blockButtons = true;
}

void FileManager::changeFileStatus(int id, FileItem::Status status) {
    bool changed = false;
    std::cout << "change" << std::endl;
    for (int i = 0; i < _dialog->_list.size(); i++) {
        if (_dialog->_list[i]->_id == id) {
            _dialog->_list[i]->_status = status;
            changed = true;
        }
    }
    if (!changed)
        std::cout << "Error! Could not find File in List." << std::endl;
    _dialog->createList(_dialog->_list);
}

void FileManager::uploadSuccess() {
    setCursor(Qt::ArrowCursor);
    _isFileDialog = false;
    /*QMessageBox msg;
    msg.setText("Upload erfolgreich!");
    msg.setInformativeText("Der Upload von " + QString::number(_uploadedFiles) + " Datei(en) auf den Server war erfolgreich.");
    msg.setStandardButtons(QMessageBox::Ok);*/
    //emit reload();
    _dialog->finished();
    _c->_net->sendMessage("getFileNames", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString(""));
    ui->statusbar->showMessage("");
}

void FileManager::closeEvent(QCloseEvent *e) {
    this->hide();
    _c->_hw->show();
    e->ignore();
}

void FileManager::BtnBackClicked() {
    if (_blockButtons)
        return;
    close();
}

FileManager::~FileManager()
{
    delete ui;
}
