#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QMainWindow>
#include "control.h"
#include "uploaddialog.h"

namespace Ui {
class FileManager;
}

class FileManager : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileManager(QWidget *parent, Control* c);
    ~FileManager();
    QString _fileList;
    void closeEvent(QCloseEvent* e) override;
    bool _isFileDialog = false;

public slots:
    void createFileList();
    void ButtonClicked();
    void BtnBackClicked();
    void addButtonClicked();
    void setFileDialog(bool b) { _isFileDialog = b; }
    void changeFileStatus(int id, FileItem::Status status);

signals:
    void reload();

private:
    friend class Network;
    Ui::FileManager *ui;
    Control* _c;
    void uploadSuccess();
    int _uploadedFiles;
    QStringList _filenames;
    bool _blockButtons;
    UploadDialog* _dialog;
};

#endif // FILEMANAGER_H
