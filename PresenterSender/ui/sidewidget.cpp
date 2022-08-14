#include "sidewidget.h"
#include <QEvent>
#include <QFont>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSizePolicy>

SideWidget::SideWidget(QString text, bool active) : QWidget {}
{
    _text = text;

    this->setMinimumHeight(70);

    _label = new QLabel(text);
    _label->setText(_text);
    //_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    //_label->setStyleSheet("background-color: red;");
    QFont font;
    font.setBold(true);
    font.setPixelSize(18);
    _label->setFont(font);

    _iconLabel = new QLabel();
    _iconLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _iconLabel->setMaximumSize(34,70);
    _iconLabel->setMinimumSize(34,70);
    //_iconLabel->setStyleSheet("background-color:blue;");

    //QSpacerItem* spacer = new QSpacerItem(5,70);

    QHBoxLayout* vlayout = new QHBoxLayout();
    this->setLayout(vlayout);
    //vlayout->addWidget(new QLabel(""));

    vlayout->addWidget(_iconLabel);
    //vlayout->addSpacerItem(new QSpacerItem(5,70));
    vlayout->addWidget(_label);
    //vlayout->addSpacerItem(new QSpacerItem(20,40));
    //vlayout->addSpacerItem(new QSpacerItem(20,70));
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    changeState(active ? State::Active : State::Inactive);
}

void SideWidget::setIcon(QPixmap pix) {
    _iconLabel->setPixmap(pix.scaledToWidth(30));
    _iconLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _icon = &pix;
}

void SideWidget::changeState(State newState)
{
    switch (newState) {
        case State::Active: {
            this->setStyleSheet("background-color: " + ACTIVE_COLOR + "; color: white;");
            break;
        }
        case State::Inactive: {
            this->setStyleSheet("background-color: " + INACTIVE_COLOR + "; color: #dddddd;");
            break;
        }
    }
    _state = newState;
}

bool SideWidget::event(QEvent *myEvent)
{
    switch(myEvent->type()) {
        case (QEvent::MouseButtonRelease): {
            emit clicked();
            break;
        }
    }
    return QWidget::event(myEvent);
}
