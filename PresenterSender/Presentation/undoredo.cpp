#include "undoredo.h"
#include <iostream>

UndoRedo::UndoRedo(QObject *parent) : QObject(parent)
{
    _currentIndex = -1;
}

void UndoRedo::newStage(QVector<Stroke*> newStage) {
    //
    // changes after undo - delete all old
    //
    if (_currentIndex == _stages.size() - 1){
        _stages.push_back(newStage);
        _currentIndex++;
        std::cout << "New Stage! - #" << newStage.size() << std::endl;
    } else {
        int count = (_stages.size() - 1) - _currentIndex;
        _stages.remove(_currentIndex + 1, count);
        _stages.push_back(newStage);
        _currentIndex++;
        std::cout << "New Stage! - #" << newStage.size() << std::endl;
        //_stages.erase(_currentIndex, _stages.size() - 1);
    }
}

QVector<Stroke*> UndoRedo::undo() {
    if (_currentIndex > 0){
        _currentIndex--;
        std::cout << "Undo #" << _currentIndex << "; Num Lines: " << _stages[_currentIndex].size() << std::endl;
        return _stages[_currentIndex];
    } else {
        std::cout << "else" << std::endl;
        return _stages[0];
    }
}

QVector<Stroke*> UndoRedo::redo() {
    if (_currentIndex == _stages.size() - 1) {
        return _stages[_currentIndex];
    } else {
        _currentIndex++;
        std::cout << "Redo #" << _currentIndex << "; Num Lines: " << _stages[_currentIndex].size() << std::endl;
        return _stages[_currentIndex];
    }
}
