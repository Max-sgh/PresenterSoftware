#include "presentation.h"
#include "ui_presentation.h"
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMovie>

Presentation::Presentation(QWidget *parent, Control* c) :
    QMainWindow(parent),
    ui(new Ui::Presentation)
{
    ui->setupUi(this);
    _c = c;

    connect(_c->_net, &Network::preparationSuccess, this, &Presentation::preparationSuccessfully);
    connect(_c->_net, &Network::gotAllPreviews, this, &Presentation::gotAllPreviews);
    connect(_c->_net, &Network::selectedPres, this, &Presentation::selectedPres);
    connect(_c->_net, &Network::closeEvent, this, &Presentation::closeEvent);
}

void Presentation::gotAllPreviews() {
    makeSelectList();
    ui->statusbar->showMessage("Done.");
}

void Presentation::preparationSuccessfully() {
    ui->statusbar->showMessage("Getting Data from Server.");
    QMovie* movie = new QMovie(":icons/spinner2.gif");
    movie->start();
    movie->setScaledSize(QSize(40,40));
    ui->label->setMovie(movie);
    //ui->scrollArea->setWidget(ui->label);
    _c->_net->getPreviews();
}

void Presentation::error(QString errorMsg) {
    QWidget* w = new QWidget();
    ui->scrollArea->setHidden(false);

    QVBoxLayout* v = new QVBoxLayout();
    QLabel* title = new QLabel();
    title->setAlignment(Qt::AlignCenter);
    title->setText("Fehler! " + errorMsg);

    QFont font;
    font.setBold(true);
    font.setPixelSize(25);
    title->setFont(font);
    v->addWidget(title);

    QPushButton* p = new QPushButton();
    p->setText("Zurück zum Hauptmenü");
    connect(p, &QPushButton::clicked, this, &Presentation::backToHome);
    v->addWidget(p);

    v->setAlignment(Qt::AlignCenter);
    w->setLayout(v);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(w);
}

void Presentation::selectedPres() {
    ui->statusbar->showMessage("Successfully set presentation on the server");
    makeClientList();
}

void Presentation::makeClientList() {
    QWidget* w = new QWidget();
    ui->scrollArea->setHidden(false);

    QVBoxLayout* v = new QVBoxLayout();
    QLabel* title = new QLabel();
    title->setAlignment(Qt::AlignCenter);
    title->setText("Client auswählen");

    QFont font;
    font.setBold(true);
    font.setPixelSize(25);
    title->setFont(font);
    v->addWidget(title);

    for (int i = 0; i < (int)_c->_net->_clientNames.size(); i++) {
        QPushButton* p = new QPushButton();
        p->setText(_c->_net->_clientNames[i]);
        p->resize(50, p->height());
        p->setMaximumSize(QSize(600, p->height()));
        QObject::connect(p, &QPushButton::clicked, this, &Presentation::selectClient);
        v->addWidget(p);
    }
    v->setAlignment(Qt::AlignCenter);
    w->setLayout(v);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(w);
}

void Presentation::selectClient() {
    QPushButton* obj = qobject_cast<QPushButton*>(QObject::sender());
    QString client = obj->text();
    _c->_net->sendMessage("setClient", QString::fromStdString(_c->_net->_id), client, QByteArray::fromStdString("null"));
    ui->scrollArea->setVisible(false);
    delete ui->scrollArea;
    connect(_c->_net, &Network::clientConnected, this, &Presentation::clientConnected);
    ui->statusbar->showMessage("Connecting...");
}

void Presentation::clientConnected() {
    ui->statusbar->showMessage("Successfully connected to Client");
    // _c->_net->sendMessage("nextSlide", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString("null"));
    // std::cout << "Hi" << std::endl;
    // ui->scrollArea->deleteLater();
    // _c->_presScreen = new PresentationScreen(_c);
    setCentralWidget(_c->_presScreen->_w);
    connect(ui->actionDraw, &QAction::triggered, _c->_presScreen, &PresentationScreen::actionDraw);
    connect(ui->actionErase, &QAction::triggered, _c->_presScreen, &PresentationScreen::actionErase);
    connect(ui->actionRedo, &QAction::triggered, _c->_presScreen, &PresentationScreen::actionRedo);
    connect(ui->actionUndo, &QAction::triggered, _c->_presScreen, &PresentationScreen::actionUndo);
    _presStarted = true;
    this->showMaximized();
}

void Presentation::makeSelectList(){
    int n = static_cast<int>(_c->_ph->_presentations.size());
    QWidget* w = new QWidget();
    QVBoxLayout* v = new QVBoxLayout();
    QLabel* title = new QLabel();
    title->setAlignment(Qt::AlignCenter);
    title->setText("Präsentation wählen");

    QFont font;
    font.setBold(true);
    font.setUnderline(true);
    font.setPixelSize(25);
    title->setFont(font);
    v->addWidget(title);

    for (int i = 0; i < n; i++) {
        QLabel* title = new QLabel();
        title->setAlignment(Qt::AlignCenter);
        title->setText("\n" + QString::fromStdString(_c->_ph->_presentations[i]));

        QFont font;
        font.setBold(true);
        font.setPixelSize(22);
        title->setFont(font);
        v->addWidget(title);

        QLabel* pic = new QLabel();
        pic->setPixmap(_c->_ph->_pixes[i]->scaled(600, 600, Qt::KeepAspectRatio));
        pic->setAlignment(Qt::AlignCenter);
        v->addWidget(pic);

        QPushButton* p = new QPushButton();
        p->setText(QString::fromStdString("\"" + _c->_ph->_presentations[i] + "\"" + " auswählen"));
        p->resize(50, p->height());
        p->setMaximumSize(QSize(600, p->height()));
        p->setObjectName(QString::number(_c->_ph->_ids[i]));
        QObject::connect(p, &QPushButton::clicked, this, &Presentation::selectPres);
        v->addWidget(p);
    }
    v->setAlignment(Qt::AlignCenter);
    w->setLayout(v);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(w);
}

void Presentation::selectPres() {
    QPushButton* obj = qobject_cast<QPushButton*>(QObject::sender());
    // QString pres = obj->text().split("\"")[1];
    _c->_net->sendMessage("setPresentation", QString::fromStdString(_c->_net->_id), obj->objectName(), QByteArray::fromStdString("null"));
    //ui->scrollArea->setVisible(false);
}

void Presentation::closeEvent(QCloseEvent* event) {
    if (_presStarted && !_c->_net->_shouldClose && _c->_net->_id.size() > 0 /*&& _c->_net->_isConnected*/) {
        if (!_hasAsked) {
            QMessageBox msg;
            msg.setText("Geänderte Daten speichern?");
            msg.setInformativeText("Die Daten der Freihandeingabe wurden verändert. Wollen Sie diese speichern? (Hinweis: es werden keine Daten auf ihrem Gerät gespeichert)");
            msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            int ret = msg.exec();
            _hasAsked = true;
            if (ret == QMessageBox::Yes) {
                _c->_net->sendMessage("saveStrokes", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString(_c->_pw->getStrokeString().toStdString()));
            } else {
                std::cout << 1 << std::endl;
                _c->_net->sendMessage("closeClient", QString::fromStdString(_c->_net->_id), "null", QByteArray::fromStdString("null"));
                emit backToHome();
            }
        }
        //this->hide();
        event->ignore();
    } else {
        //_c->_net->sendMessage("closeClient", QString::fromStdString(_c->_net->_id), "", QByteArray::fromStdString(""));
        emit backToHome();
        event->ignore();
        // event->accept();
    }
}

Presentation::~Presentation()
{
    delete ui;
}
