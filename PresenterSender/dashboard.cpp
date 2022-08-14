#include "dashboard.h"
#include "ui_dashboard.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include "ui/sidewidget.h"

Dashboard::Dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Dashboard)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: white;");
    _centralW = new QWidget(this);
    // 10 x 10
    QGridLayout* layout = new QGridLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(22);
    _centralW->setLayout(layout);

    _logo = new QLabel("Logo");
    _userIcon = new QLabel();
    _userIcon->setPixmap(QPixmap(":/icons/user.png"));
    //QLabel* logout = new QLabel("Logout");
    _menubar = new QWidget(this);
    QHBoxLayout* hlayout = new QHBoxLayout(_menubar);

    hlayout->addWidget(_logo);
    hlayout->addSpacerItem(new QSpacerItem(40,20,QSizePolicy::Expanding, QSizePolicy::Minimum));
    hlayout->addWidget(_userIcon);
    _menubar->setLayout(hlayout);
    _menubar->setStyleSheet("background-color: #3465a4; margin: 0;");
    layout->addWidget(_menubar, 0, 0, 1, 10);

    _sideBar = new QWidget(this);
    _sideBar->setStyleSheet("background-color: #cccccc; margin: 0");
    _vlayout = new QVBoxLayout(_sideBar);
    _sideBar->setLayout(_vlayout);

    SideWidget* sw = new SideWidget("Test", true);
    connect(sw, &SideWidget::clicked, this, &Dashboard::newActive);
    _widgets.append(sw);
    _currentActive = sw;
    sw->setIcon(QPixmap(":icons/trash.png"));

    SideWidget* sw2 = new SideWidget("Presentation", false);
    connect(sw2, &SideWidget::clicked, this, &Dashboard::newActive);
    _widgets.append(sw2);
    sw2->setIcon(QPixmap(":icons/presentation.png"));

    _vlayout->addWidget(sw);
    _vlayout->addWidget(sw2);
    _vlayout->setSpacing(0);
    _vlayout->setContentsMargins(0,0,0,0);
    _vlayout->addSpacerItem(new QSpacerItem(40,20,QSizePolicy::Ignored, QSizePolicy::Expanding));
    layout->addWidget(_sideBar, 1, 0, 9, 1);

    setCentralWidget(_centralW);
}

Dashboard::~Dashboard()
{
    delete ui;
}

void Dashboard::newActive()
{
    SideWidget* newActive = qobject_cast<SideWidget*>(sender());
    if (newActive == _currentActive) {
        return;
    }
    newActive->changeState(SideWidget::State::Active);
    _currentActive->changeState(SideWidget::State::Inactive);
    _currentActive = newActive;
}
