#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QDialog>
#include "fileitem.h"
#include <QCloseEvent>

namespace Ui {
class UploadDialog;
}

class UploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadDialog(QWidget *parent = nullptr);
    ~UploadDialog();
    void createList(QList<FileItem*> list);
    QList<FileItem *> _list;

public slots:
    void finished();
    void closeEvent(QCloseEvent* e) override;

private:
    Ui::UploadDialog *ui;
    bool _finished = false;
};

#endif // UPLOADDIALOG_H
