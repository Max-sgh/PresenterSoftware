#include "filehandler.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <iostream>
#include <QImage>
#include <QPainter>
#include <QPrinter>
#include <QtPdf>

FileHandler::FileHandler(QObject *parent) : QObject(parent)
{

}

bool FileHandler::convertPDF(QString file, int* i, DbManager* dbm) {
//    QFileInfo fi(file);
//    QString s = QString::fromStdString(files->getLastRow()._fields[2].s);
//    s.remove(0, QString("Data/").length());
//    for (int i = 0; i < s.length(); i++){
//        if (s.at(i) == '0') {
//            s.remove(i);
//        }
//    }
//    int num = s.toInt();
//    QString dirname = "Data/";
//    for (int i = 0; i < 4 - QString::number(num + 1).size(); i++) {
//        dirname += "0";
//    }
//    dirname += QString::number(num + 1);

    QFileInfo fi(file);
    //QString s = QString::fromStdString(files->getLastRow()._fields[2].s);
    QList<QVariant>* list = dbm->singleSelect("SELECT folderName FROM files ORDER BY ID DESC LIMIT 1");
    if (list->at(0).toBool() == false) {
        std::cerr << "DB-Error: FileHandler::convertPDF() " << list->at(1).toString().toStdString() << std::endl;
        return false;
    }
    QString s = QString::number(list->at(2).toInt());
    delete list;
    for (int i = 0; i < s.length(); i++){
        if (s.at(i) == '0') {
            s.remove(i);
        }
    }
    int num = s.toInt();
    QString dirnameShort;
    for (int i = 0; i < 4 - QString::number(num + 1).size(); i++) {
        dirnameShort += "0";
    }
    dirnameShort += QString::number(num + 1);
    QString dirname = "Data/" + dirnameShort;
    std::cout << "convertPDF " << dirname.toStdString() << std::endl;

    /*QStringList arguments;
    arguments << "render" << "--input='" + dirname + "/" + fi.fileName() + "'";
    QProcess::execute("sh", QStringList() << "Data/Tools/PDF.sh" << dirname << fi.fileName());
    bool exists = true;
    int i2 = 1;
    while (exists) {
        QFile f(dirname + "/" + fi.completeBaseName() + "-" + QString::number(i2) + ".jpg");
        if (!f.exists()) {
            exists = false;
            break;
        } else {
            if (f.open(QIODevice::ReadOnly)) {
                f.copy(dirname + "/" + "Slide" + QString::number(i2) + ".jpg");
                f.remove();
                f.close();
                i2++;
            } else {
                std::cerr << "Error opening File: " << fi.fileName().toStdString() << "; Error: " << f.errorString().toStdString() << std::endl;
                return false;
            }
        }
    }*/
    QPdfDocument* document = new QPdfDocument();
    document->load(dirname + "/" + fi.fileName());
    std::cout << dirname.toStdString() + "/" + fi.fileName().toStdString() << std::endl;
    for (int i = 0; i < document->pageCount() + 1; i++) {
        QPdfDocumentRenderOptions options;
        options.setRenderFlags(QPdf::RenderImageAliased);
        QImage img = document->render(i, document->pageSize(i).toSize(), options);
        img.save(dirname + "/Slide" + QString::number(i + 1) + ".jpg", "JPG", 98);
        std::cout << dirname.toStdString() + "/Slide" << i + 1 << ".jpg" << std::endl;
    }
    *i = document->pageCount() + 1;
    return true;
}
bool FileHandler::convertFile(Room *r, QString file, DbManager* dbm) {
    QFileInfo fi(file);
    QList<QVariant>* list = dbm->singleSelect("SELECT folderName FROM files ORDER BY ID DESC LIMIT 1");
    if (list->at(0).toBool() == false) {
        std::cerr << "DB-Error: FileHandler::convertPDF() " << list->at(1).toString().toStdString() << std::endl;
        return false;
    }
    QString s = QString::number(list->at(2).toInt());
    delete list;
    for (int i = 0; i < s.length(); i++){
        if (s.at(i) == '0') {
            s.remove(i);
        }
    }
    int num = s.toInt();
    //std::cout << "Num: " << num << std::endl;
    QString dirnameShort = "";
    for (int i = 0; i < 4 - QString::number(num + 1).size(); i++) {
        dirnameShort += "0";
    }
    dirnameShort += QString::number(num + 1);
    QString dirname = "Data/" + dirnameShort;
    std::cout << dirname.toStdString() << std::endl;
    QDir().mkdir(dirname);
    QStringList extensions;
    extensions << "odp" << "pptx";
    if (fi.suffix() == "pdf") {
        return convertPDFWithEntry(r, file, dbm);
    } else if (extensions.contains(fi.suffix())) {
        QFile f(file);
        if (f.open(QIODevice::ReadOnly)) {
            f.copy(dirname + "/" + fi.fileName());
            f.remove();
            f.close();
        }
        QProcess::execute("sh", QStringList() << "Data/Tools/Document.sh" << dirname + "/*." + fi.suffix() << dirname);
        int numS = 0;
        if (!convertPDF(fi.completeBaseName() + ".pdf", &numS, dbm)) {
            return false;
        } else {
            int id = num + 1;
            std::string username = r->_username;
            QString folder = dirnameShort;
            QString name = fi.completeBaseName();
            QString extension = fi.suffix();

            /*DatabaseRow* row = new DatabaseRow();
            row->createField(id, "");
            row->createField(0, username);
            row->createField(0, folder.toStdString());
            row->createField(0, name.toStdString());
            row->createField(0, extension.toStdString());

            files->addRow(row);
            CDBFilehandler* fh = new CDBFilehandler("Data/files.cdb");

            fh->saveDB(files);
            delete fh;

            delete row;*/
            int numEffectedRows = dbm->insert("INSERT INTO files (user, folderName, name, type) VALUES ('"
                                              + QString::fromStdString(username) + "', '" + folder + "', '" + name + "', '" + extension + "');");
            if (numEffectedRows != 1) {
                std::cerr << "DB-Error! FileHandler::convertFile()" << std::endl;
                return false;
            }
            /*row = new DatabaseRow();
            row->createField(id + 1, "");
            row->createField(0, username);
            row->createField(0, folder.toStdString());
            row->createField(0, name.toStdString());
            row->createField(numS - 1, "");
            presentations->addRow(row);

            fh = new CDBFilehandler("Data/presentations.cdb");
            fh->saveDB(presentations);*/
            numEffectedRows = dbm->insert("INSERT INTO presentations (user, folderName, name, numSlides) VALUES ('"
                                              + QString::fromStdString(username) + "', '" + folder + "', '" + name + "', '" + QString::number(numS - 1) + "');");
            if (numEffectedRows != 1) {
                std::cerr << "DB-Error! FileHandler::convertFile()" << std::endl;
                return false;
            }
            //delete fh;
        }
        std::cout << "Successfully converted File!" << std::endl;
        return true;
    }
    else {
        std::cout << fi.suffix().toStdString() << std::endl;
        return true;
    }
}

bool FileHandler::convertPDFWithEntry(Room* r, QString file, DbManager* dbm) {
    QFileInfo fi(file);
    QList<QVariant>* list = dbm->singleSelect("SELECT folderName FROM files ORDER BY ID DESC LIMIT 1");
    if (list->at(0).toBool() == false) {
        std::cerr << "DB-Error: FileHandler::convertPDF() " << list->at(1).toString().toStdString() << std::endl;
        return false;
    }
    QString s = QString::number(list->at(2).toInt());
    delete list;
    for (int i = 0; i < s.length(); i++){
        if (s.at(i) == '0') {
            s.remove(i);
        }
    }
    int num = s.toInt();
    QString dirnameShort;
    for (int i = 0; i < 4 - QString::number(num + 1).size(); i++) {
        dirnameShort += "0";
    }
    dirnameShort += QString::number(num + 1);
    QString dirname = "Data/" + dirnameShort;
    QDir().mkdir(dirname);

    QFile f(file);
    if (f.open(QIODevice::ReadOnly)) {
        f.copy(dirname + "/" + fi.fileName());
        f.remove();
        f.close();
    }
    // QStringList arguments;
    QPdfDocument* document = new QPdfDocument();
    document->load(dirname + "/" + fi.fileName());
    for (int i = 0; i < document->pageCount() + 1; i++) {
        QPdfDocumentRenderOptions options;
        options.setRenderFlags(QPdf::RenderImageAliased);
        QImage img = document->render(i, document->pageSize(i).toSize(), options);
        img.save(dirname + "/Slide" + QString::number(i + 1) + ".jpg", "JPG", 98);
    }
    int i = document->pageCount() + 1;

    int id = num + 1;
    std::string username = r->_username;
    QString folder = dirnameShort;
    QString name = fi.completeBaseName();
    QString extension = fi.suffix();

    /*DatabaseRow* row = new DatabaseRow();
    row->createField(id, "");
    row->createField(0, username);
    row->createField(0, folder.toStdString());
    row->createField(0, name.toStdString());
    row->createField(0, extension.toStdString());

    files->addRow(row);
    CDBFilehandler* fh = new CDBFilehandler("Data/files.cdb");
    fh->saveDB(files);
    delete fh;

    delete row;*/
    int numEffectedRows = dbm->insert("INSERT INTO files (user, folderName, name, type) VALUES ('"
                                      + QString::fromStdString(username) + "', '" + folder + "', '" + name + "', '" + extension + "');");
    if (numEffectedRows != 1) {
        std::cerr << "DB-Error! FileHandler::convertFile()" << std::endl;
        return false;
    }

    /*row = new DatabaseRow();
    row->createField(id, "");
    row->createField(0, username);
    row->createField(0, folder.toStdString());
    row->createField(0, name.toStdString());
    row->createField(i - 1, "");
    presentations->addRow(row);

    fh = new CDBFilehandler("Data/presentations.cdb");
    fh->saveDB(presentations);
    delete fh;*/
    numEffectedRows = dbm->insert("INSERT INTO presentations (user, folderName, name, numSlides) VALUES ('"
                                      + QString::fromStdString(username) + "', '" + folder + "', '" + name + "', '" + QString::number(i - 1) + "');");
    if (numEffectedRows != 1) {
        std::cerr << "DB-Error! FileHandler::convertFile()" << std::endl;
        return false;
    }

    std::cout << "Successfully converted File!" << std::endl;
    return true;
}

bool FileHandler::deleteFile(QString dirname, DbManager* dbm) {
    QDir dir("Data/" + dirname);
    std::cout << dirname.toStdString() << std::endl;
    if (!dir.exists()) {
        std::cerr << "Directory does not exists!" << std::endl;
        return false;
    } else {
        dir.removeRecursively();
        /*if (!files->deleteRow(files->getRowsByValue("folderName", dirname.toStdString())[0]._fields[0].i)) {
            std::cout << "false1" << std::endl;
            return false;
        }*/
        int numEffectedRows = dbm->remove("DELETE FROM files WHERE foldername='" + dirname + "'");
        if (numEffectedRows != 1) {
            std::cerr << "DB-Error: FileHandler::deleteFile" << std::endl;
            return false;
        }
        /*if (!presentations->deleteRow(presentations->getRowsByValue("folderName", dirname.toStdString())[0]._fields[0].i)) {
            std::cout << "false2" << std::endl;
            return false;
        }*/
        numEffectedRows = dbm->remove("DELETE FROM presentations WHERE foldername='" + dirname + "'");
        if (numEffectedRows != 1) {
            std::cerr << "DB-Error: FileHandler::deleteFile" << std::endl;
            return false;
        }
        /*CDBFilehandler* fh = new CDBFilehandler("Data/files.cdb");
        fh->saveDB(files);
        delete fh;

        fh = new CDBFilehandler("Data/presentations.cdb");
        fh->saveDB(presentations);*/
        std::cout << "Successfully deleted File!" << std::endl;
        return true;
    }
    return true;
}

QImage FileHandler::renderSlide(QImage img, QString strokes)
{
    QStringList l = strokes.split("stroke:");
    l.removeAt(0);

    for (int i = 0; i < l.size(); i++) {
        QString line = l[i];
        line = line.left(line.lastIndexOf(":"));
        QVector<QPoint> points;
        QStringList l = line.split(":");

        QString hex = l[0];
        l.removeAt(0);
        int width = l[0].toInt();
        l.removeAt(0);

        for (int j = 0; j < l.size(); j += 2) {
            QPoint p(l[j].toInt(), l[j+1].toInt());
            points.push_back(p);
        }

        QPolygon pol(points);
        QPainter painter(&img);
        painter.drawImage(QPoint(0,0), img);
        painter.setPen(QPen(QColor(hex), width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPolyline(pol);
    }
    return img;
}

void FileHandler::exportToPDF(QVector<QImage> imgs, QString filename)
{

    if (imgs.size() == 0)
        return;
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setPaperSize(QSizeF(imgs[0].size().width(), imgs[0].size().height()), QPrinter::DevicePixel);
    QPainter painter;
    painter.begin(&printer);
    for (int i = 0; i < imgs.size(); i++) {
        painter.drawImage(QPoint(0,0), imgs[i]);
        if (imgs.size() == i + 1)
            continue;
        printer.newPage();
    }
    painter.end();
    std::cout << "Successfully converted File!" << std::endl;
    return;
}
