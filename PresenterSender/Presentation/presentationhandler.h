#ifndef PRESENTATIONHANDLER_H
#define PRESENTATIONHANDLER_H

#include <string>
#include <QPixmap>
#include <vector>
#include <QObject>
#include <iostream>
#include "../control.h"

class Control;

class PresentationHandler : public QObject
{
Q_OBJECT
public:
    PresentationHandler(QObject* parent, Control* c);
    std::vector<std::string> _presentations;
    std::vector<int> _ids;
    std::vector<QPixmap*> _pixes;
    //void getImage(std::string name);
    //void getImageAsString(std::string name);
    //void getPreview(std::string name);
    Control* _c;

private:
    std::vector<std::string> iterateArgs(std::string origin);
};

#endif // PRESENTATIONHANDLER_H
