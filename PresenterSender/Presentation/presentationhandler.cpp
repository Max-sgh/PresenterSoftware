//#include "mainwindow.h"
#include "presentationhandler.h"
//#include "network.h"

#include <sstream>

PresentationHandler::PresentationHandler(QObject* parent, Control* c)
{
    delete parent;
    _c = c;
}
