#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <QObject>
#include <QVector>
#include <QPolygon>
#include "stroke.h"

class UndoRedo : public QObject
{
    Q_OBJECT
public:
    explicit UndoRedo(QObject *parent = nullptr);
    void newStage(QVector<Stroke*>);
    QVector<Stroke*> undo();
    QVector<Stroke*> redo();

private:
    int _currentIndex;
    QVector<QVector<Stroke*>> _stages;
};


#endif // UNDOREDO_H
