#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QLabel>

class clickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit clickableLabel(QString text, QObject *parent = 0) : QLabel(text) {}
    ~clickableLabel() {}

signals:
    void clicked();

protected:
    void mousPessEvent(QMouseEvent* event) { emit clicked(); }

};

#endif // CLICKABLELABEL_H
