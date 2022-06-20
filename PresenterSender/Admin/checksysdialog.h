#ifndef CHECKSYSDIALOG_H
#define CHECKSYSDIALOG_H

#include <QDialog>
#include "network.h"
#include "checkitem.h"

class Network;

//
// Datenbanken überprüfen (jede einzeln)
// Ordner auf doppelte
// Erreichbarkeit aus dem Internet
// Cache leeren (falls keine aktuellen Verbindungen)
//

namespace Ui {
class CheckSysDialog;
}

class CheckSysDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckSysDialog(Network* net, QWidget *parent = nullptr);
    ~CheckSysDialog();

public slots:
    void checkDone(QString action, QString status);
    void nextElement();
    void finished();

private:
    Ui::CheckSysDialog *ui;
    Network* _net;
    QList<CheckItem*> _items;
    int _checked = 0;
    void createList(QList<CheckItem*> list);
};

#endif // CHECKSYSDIALOG_H
