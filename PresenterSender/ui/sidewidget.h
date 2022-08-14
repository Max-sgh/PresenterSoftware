#ifndef SIDEWIDGET_H
#define SIDEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <iostream>
#include <QEvent>

class SideWidget : public QWidget
{
    Q_OBJECT
public:
    SideWidget(QString text, bool active);
    enum State { Active, Inactive};
    void changeState(State newState);
    void setIcon(QPixmap pix);

signals:
    void clicked();

protected:
    bool event(QEvent* myEvent);

private:
    QString _text;
    QPixmap* _icon;
    State _state;
    QLabel* _label;
    QLabel* _iconLabel;

    const QString ACTIVE_COLOR = "#3465a4"; //"#b2b2b2";
    const QString INACTIVE_COLOR = "#b2b2b2"; //"#cccccc";

    //Dashboard* _dashboard;
};

#endif // SIDEWIDGET_H
