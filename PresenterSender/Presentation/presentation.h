#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <QMainWindow>
#include "../control.h"

namespace Ui {
class Presentation;
}

class Presentation : public QMainWindow
{
    Q_OBJECT

public:
    explicit Presentation(QWidget *parent, Control* c);
    ~Presentation();

public slots:
    void preparationSuccessfully();
    void gotAllPreviews();
    void selectedPres();
    void clientConnected();
    void closeEvent(QCloseEvent* event) override;
    void error(QString errorMsg);

private:
    Ui::Presentation *ui;
    Control* _c;

    QPixmap* _pix;
    void makeSelectList();
    void selectPres();
    void makeClientList();
    void selectClient();
    bool _hasAsked = false;
    bool _presStarted = false;

signals:
    void backToHome();
};

#endif // PRESENTATION_H
