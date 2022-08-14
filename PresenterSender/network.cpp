#include "control.h"
#include "settingsdialog.h"

#include <iostream>
#include <map>
#include <iterator>
#include <QDebug>
#include <QByteArray>
#include <QImage>
#include <QPixmap>
#include <sstream>
#include <fstream>
#include <QTimer>
#include <QThread>
#include <QFile>
#include <thread>
#include <QMessageBox>
#include <QStringList>
#include <QFileDialog>

Network::Network(QObject *parent, std::string hostname, int port, Control* c) : QObject(parent)
{
    _hostname = hostname;
    _port = port;
    _c = c;
}

Network::~Network() {
}

bool Network::checkServer() {
    bool connected = connectToHost();
    if (connected)
        _socket->close();
    return connected;
}

bool Network::initializeServer() {
    _c->_splash->showMessage("Verbinde zum Server...", Qt::AlignRight | Qt::AlignBottom, Qt::white);
    if (!connectToHost()) {
        std::cout << "Server Error!" << std::endl;
        //_c->_splash->hide();
        _c->_splash->clearMessage();
        QMessageBox::warning(new QWidget(), "Keine Verbindung zum Server möglich!", "Es konnte keine Verbindung mit dem Server hergestellt werden.\
                             Prüfen Sie die Einstellungen und die Netzwerkverbindung des Geräts und versuchen Sie es später erneut. Sollte das Problem\
weiterhin auftreten kontaktieren Sie den zuständigen Administrator");
        //exit(1);
        SettingsDialog* dlg = new SettingsDialog();
        dlg->setTexts(_c->_serverName, _c->_serverIP);
        dlg->exec();
        //_c->_splash->show();
        _c->_splash->showMessage("Überprüfe Eingaben...", Qt::AlignRight | Qt::AlignBottom, Qt::white);
        dlg->hide();
        // dlg->_wait->setVisible(true);
        // dlg->show();
        if (dlg->_rejected) {
            //exit(0);
            delete dlg;
            exit(0);
        }
        QTcpSocket* socket = new QTcpSocket();
        socket->connectToHost(dlg->_serverIP, 8083);
        if (socket->waitForConnected(3000)) {
            QMessageBox::information(new QWidget(), "Einstellungen korrekt!", "Die Einstellungen wurde erfolgreich überprüft und werden nun gespeichert.",
                                     QMessageBox::Ok);
            _c->saveSettings("settings.xml", dlg->_serverName, dlg->_serverIP);
            _hostname = dlg->_serverIP.toStdString();
        } else {
            QMessageBox::critical(new QWidget(), "Einstellungen nicht korrekt!", "Die vorgenommen Änderungen sind nicht korrekt, es konnte mit diesen \
                                                                                 keine Verbindungen zum Server hergestellt werden. Bitte überprüfen Sie \
                                                                                 ihre Eingaben und versuchen Sie es erneut.", QMessageBox::Ok);
            delete dlg;
            exit(0);
        }

    }
    return true;
}

void Network::login(std::string username, std::string password){
    _c->_splash->showMessage("Logge ein...", Qt::AlignRight | Qt::AlignBottom, Qt::white);
    _username = username;
    _password = password;

    QDataStream socketStream(_socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    QByteArray header;
    header.prepend(QString::fromStdString("instruction:login,arg1:" + username + ",args2:" + password + ";").toUtf8());
    header.resize(128);

    QByteArray byteArray = "null";
    byteArray.prepend(header);

    socketStream << byteArray;
}

void Network::sendMessage(QString instruction, QString arg1, QString arg2, QByteArray data) {
    QTcpSocket socket;
    socket.connectToHost(QString::fromStdString(_hostname), _port);
    if (socket.waitForConnected(3000) && _socket->isOpen()) {
        QDataStream socketStream(_socket);
        socketStream.setVersion(QDataStream::Qt_5_12);

        QByteArray header;
        header.prepend(QString::fromStdString("instruction:" + instruction.toStdString() + ",arg1:" + arg1.toStdString() + ",args2:" + arg2.toStdString() + ";").toUtf8());
        header.resize(128);

        QByteArray byteArray = data;
        byteArray.prepend(header);

        socketStream << byteArray;
    } else {
        QMessageBox::warning(new QWidget(), "Keine Verbindung zum Server!", "Es kann keine Verbindung zum Server hergestellt werden. Starten Sie die App neu und probieren Sie es erneut.", QMessageBox::Ok);
        exit(0);
    }
    socket.deleteLater();
}

void Network::getPreviews() {
    sendMessage("getPreview", QString::fromStdString(_c->_net->_id), QString::number(_c->_ph->_ids[0]), QByteArray::fromStdString("null"));
}

void Network::loginSuccess(QString completed) {
    if (completed == "login") {
        //sendMessage("createRoom", QString::fromStdString(_username), "null", QByteArray::fromStdString("null"));
        sendMessage("syncNetworkInformation", "", "", "");
    }
    /*if (completed == "getPresentations") {
        sendMessage("getPresentations", QString::fromStdString(_username), "null", QByteArray::fromStdString("null"));
    }*/
    if (completed == "getPresentations") {
        emit preparationSuccess();
    }
}

void Network::readRemoteSocket() {
    QByteArray buffer;
    QDataStream socketStream(_remoteSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    socketStream.startTransaction();
    socketStream >> buffer;

    if (!socketStream.commitTransaction()) {
        std::cout << "waiting for Data!" << std::endl;
        return;
    }

    QString header = buffer.mid(0,128);
    QString instruction = header.split(",")[0].split(":")[1];

    if (instruction == "messagse")
        return;
    QString success = header.split(",")[1].split(":")[1];
    QString arg1 = header.split(",")[2].split(":")[1];
    QString arg2 = header.split(",")[3].split(":")[1].split(";")[0];

    buffer = buffer.mid(128);
    if (instruction == "checkForRemote") {
        if (success == "success") {
            std::cout << "Connection can be initialized over local network!" << std::endl;
            _socket->close();
            _socket->disconnect();
            _socket->deleteLater();
            _socket = _remoteSocket;
            _socket->disconnect();
            connect(_socket, &QTcpSocket::readyRead, this, &Network::readSocket);
            sendMessage("createRoom", QString::fromStdString(_username), "null", QByteArray::fromStdString("null"));
        } else {
            std::cout << "The server is not in the local network and so the connection must be established over remote socket!" << std::endl;
            _remoteSocket->close();
            _remoteSocket->disconnect();
            _remoteSocket->deleteLater();
            emit setRemote();
            sendMessage("createRoom", QString::fromStdString(_username), "null", QByteArray::fromStdString("null"));
        }
    } else {
        std::cout << "Cannot decode incoming message - " << instruction.toStdString() << std::endl;
    }
}

void Network::readSocket() {
    QByteArray buffer;
    QDataStream socketStream(_socket);
    socketStream.setVersion(QDataStream::Qt_5_12);

    socketStream.startTransaction();
    socketStream >> buffer;

    if (!socketStream.commitTransaction()) {
        // std::cout << "waiting for Data!" << std::endl;
        return;
    }

    QString header = buffer.mid(0,128);
    QString instruction = header.split(",")[0].split(":")[1];

    if (instruction == "messagse")
        return;

    QString success = header.split(",")[1].split(":")[1];
    QString arg1 = header.split(",")[2].split(":")[1];
    QString arg2 = header.split(",")[3].split(":")[1].split(";")[0];

    buffer = buffer.mid(128);
    // std::cout << header.toStdString() << std::endl;
    if (instruction == "login") {
        if (success == "success") {
            std::cout << "Login successfull" << std::endl;
            std::cout << arg1.toStdString() << std::endl;;
            if (arg1 == "true") {
                std::cout << "admin" << std::endl;
                _isAdmin = true;
                emit isAdmin();
            }
            loginSuccess("login");
        } else {
            std::cout << "Error! Can´t login on Server. Error-Message: " + buffer.toStdString() << std::endl;
        }
    }
    else if (instruction == "getPresentations") {
        if (success == "failed") {
            emit error(QString::fromStdString(buffer.toStdString()));
            return;
        }
        QList<QByteArray> presentations = buffer.split(';');
        QVector<QString> pres;
        QVector<int> ids;
        foreach (QString s, presentations) {
            pres.push_back(s.split(",")[0]);
            ids.push_back(s.split(",")[s.split(",").length() - 1].toInt());
        }
        for (int i = 0; i < pres.length(); i++) {
            _c->_ph->_presentations.push_back(pres[i].toStdString());
            _c->_ph->_ids.push_back(ids[i]);
        }
        loginSuccess("getPresentations");
    }
    else if (instruction == "syncNetworkInformation") {
        _remoteSocket = new QTcpSocket();
        connect(_remoteSocket, &QTcpSocket::readyRead, this, &Network::readRemoteSocket);
        _remoteSocket->connectToHost(arg1, 8083);
        if (_remoteSocket->waitForConnected(3000) && _remoteSocket->isOpen()) {
            QDataStream socketStream(_remoteSocket);
            socketStream.setVersion(QDataStream::Qt_5_12);

            QByteArray header;
            header.prepend(QString::fromStdString("instruction:checkForRemote,arg1:,args2:;").toUtf8());
            header.resize(128);

            QByteArray byteArray = buffer;
            byteArray.prepend(header);

            socketStream << byteArray;
        } else {
            std::cout << "Can´t connect to server! - Enabling remote connection!" << std::endl;
            _remoteSocket->close();
            _remoteSocket->disconnect();
            _remoteSocket->deleteLater();
            emit setRemote();
            sendMessage("createRoom", QString::fromStdString(_username), "null", QByteArray::fromStdString("null"));
        }
    }
    else if (instruction == "createRoom") {
        if (success == "success") {
            std::cout << "Room created - " << arg1.toStdString() << std::endl;
            _id = arg1.toStdString();
            _c->_hw->show();
            _c->_splash->finish(_c->_hw);
            // loginSuccess("createRoom");
        }
    }
    else if (instruction == "getPreview") {
        if (success == "success") {
            std::cout << "got Image " << arg1.toStdString() << " " << buffer.length() << std::endl;
            QPixmap* pix = new QPixmap();
            pix->loadFromData(buffer);
            //pix->save(arg2 + ".jpg");
            if (std::find(_newNames.begin(), _newNames.end(), arg1) == _newNames.end()) {
                _newPixes.push_back(pix);
                _newNames.push_back(arg1);
                emit gotPreview();
            }
        }
        else {
            std::cout << "Error get Preview - " << buffer.toStdString() << std::endl;
        }
    }
    else if (instruction == "setPresentation") {
        if (success == "success") {
            //
            // TODO: real split with more than one client
            //
            std::cout << buffer.toStdString() << std::endl;
            QStringList list = QString::fromStdString(buffer.toStdString()).split(",");
            for (int i = 0; i< list.size(); i++) {
                _clientNames.push_back(list[i]);
            }
            list.clear();
            //clean
            _newPixes.clear();
            _newNames.clear();
            _c->_ph->_pixes.clear();
            emit selectedPres();
        } else {
            std::cout << buffer.toStdString() << std::endl;
            if (buffer.toStdString().compare("no avaiable") == 0) {
                int ret = QMessageBox::critical(new QWidget(), "No Clients available", "Currently there are no Clients avaiable. Please try again later.", QMessageBox::Ok, QMessageBox::Ok);
                if (ret == QMessageBox::Ok){
                    //exit(1);
                    _c->_pres->hide();
                    _c->_hw->show();
                }
            }
            std::cout << success.toStdString() << std::endl;
        }
    }
    else if (instruction == "setClient") {
        if (success == "success") {
            std::cout << "Connected to Client!" << std::endl;
            emit clientConnected();
            //std::cout << "1" << std::endl;
            sendMessage("nextSlide", QString::fromStdString(_id), "", QByteArray::fromStdString(""));
            //std::cout << "1" << std::endl;
            _isConnected = true;
            //std::cout << "1" << std::endl;
        } else {
            std::cout << "Error connecting Client!" << std::endl;
            QMessageBox::critical(new QWidget(), arg1, QString::fromStdString(buffer.toStdString()), QMessageBox::Ok, QMessageBox::Ok);
            emit closeEvent(new QCloseEvent());
        }
    }
    else if (instruction == "Slide") {
        std::cout << instruction.toStdString() << buffer.size() << std::endl;
        if (arg1 == "last") {
            emit _c->_pw->endReached();
            return;
        }
        QPixmap* pix = new QPixmap();
        pix->loadFromData(buffer);
        _origImage = pix->toImage();
        // pix->save("test2.jpg");
        _c->_presScreen->setSlide(pix);
        std::cout << header.toStdString() << std::endl;
        if (arg2 == "true") {
            sendMessage("getStrokes", QString::fromStdString(_id), arg1, QByteArray::fromStdString(""));
        }
    }
    else if (instruction == "closeRoom") {
        exit(0);
    }
    else if (instruction == "black") {
        if (arg1 == "true") {
            _c->_pw->saveCurrentImage();
            _c->_pw->fillWithColor(0,0,0);
            std::cout << "fill" << std::endl;
        } else if (arg1 == "false") {
            _c->_pw->restoreCurrentImage();
            std::cout << 2 << std::endl;
        } else {
            std::cout << "Error" << std::endl;
        }
    }
    else if (instruction == "getStrokes") {
        std::cout << success.toStdString() << std::endl;
        if (success == "success") {
            QString strokes = QString::fromStdString(buffer.toStdString());
            _unsavedStrokes = true;
            QImage img = _c->_pw->loadStrokes(strokes);
            QPixmap pix2 = QPixmap::fromImage(img);
            _c->_presScreen->setSlide(&pix2);
            _c->_pw->sendStrokes();
        }
    }
    else if (instruction == "saveStrokes") {
        // sendMessage("closeRoom", QString::fromStdString(_id), "null", QByteArray::fromStdString(""));
        sendMessage("closeClient", QString::fromStdString(_id), "", QByteArray::fromStdString(""));
        emit _c->_pres->backToHome();
    }
    else if (instruction == "getFileNames") {
        if (success == "success") {
            //_c->_hw->reloadDat();
            _c->_fm->_fileList = QString::fromStdString(buffer.toStdString());
            emit gotFileList();
        }
    }
    else if (instruction == "uploadFile") {
        std::cout << success.toStdString() << std::endl;
        if (success == "success") {
            _uploadConfirmed++;
            emit changeFileStatus(arg1.toInt(), FileItem::Status::Success);
            if (_uploadConfirmed == _c->_fm->_uploadedFiles) {
                _c->_fm->_isFileDialog = false;
                _c->_fm->uploadSuccess();
                _uploadConfirmed = 0;
            }
        } else {
            emit changeFileStatus(arg1.toInt(), FileItem::Status::Error);
            _c->_fm->_isFileDialog = false;
            QMessageBox::warning(new QWidget, "Upload gescheitert!", "Der Upload von " \
                                 + QString::number(_c->_fm->_uploadedFiles) + " Datei(en) ist bei Datei " + QString::number(_uploadConfirmed - 1) \
                                 + "gescheitert! Bitte versuchen Sie es später erneut", QMessageBox::Ok);
        }
    }
    else if (instruction == "closeClient") {
        std::cout << instruction.toStdString() << std::endl;
    }
    else if (instruction == "deleteFile") {
        std::cout << success.toStdString() << std::endl;
        if (success != "success") {
            QMessageBox::warning(new QWidget(), "Fehler beim Löschen der Datei!", "Die Datei konnte auf dem Server "
                                                                         "nicht gelöscht werden. Bitte kontaktieren Sie den "
                                                                         "Administrator des Servers, denn es könnte sich hier"
                                                                         " um einen schwerwiegenden Fehler der Datenbank handeln.");
        }
        sendMessage("getFileNames", QString::fromStdString(_id), "null", QByteArray::fromStdString(""));
    }
    else if (instruction == "exportToPDF") {
        std::cout << success.toStdString() << std::endl;
        // _c->_fm->_isFileDialog = true;
        QString filename = QFileDialog::getSaveFileName(new QWidget(), tr("Datei speichern"), QDir::homePath(), tr("PDF Dateien (*.pdf)"));
        QFile f(filename);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(buffer);
            f.close();
            _c->_fm->_blockButtons = false;
            _c->_fm->setCursor(Qt::ArrowCursor);
            // _c->_fm->_isFileDialog = false;
            QMessageBox::information(new QWidget(), "Datei erfolgreich gespeichert!", "Ihre Datei wurde erfolgreich gespeichert.");
        } else {
            _c->_fm->_blockButtons = false;
            _c->_fm->setCursor(Qt::ArrowCursor);
            _c->_fm->_isFileDialog = false;
        }
    }
    else if (instruction == "getUserList") {
        _c->_um->_user = QString::fromStdString(buffer.toStdString());
        _c->_um->makeUserList();
    }
    else if (instruction == "changeUser") {
        std::cout << "changes Saved" << std::endl;
        _c->_um->setCursor(Qt::WaitCursor);
        sendMessage("getUserList", QString::fromStdString(_id), "", "");
    }
    else if (instruction == "deleteUser") {
        std::cout << "successfully deleted" << std::endl;
        _c->_um->setCursor(Qt::WaitCursor);
        sendMessage("getUserList", QString::fromStdString(_id), "", "");
    }
    else if (instruction == "newUser") {
        std::cout << "successfully added" << std::endl;
        _c->_um->setCursor(Qt::WaitCursor);
        sendMessage("getUserList", QString::fromStdString(_id), "", "");
    }
    else if (instruction == "resetPassword") {
        if (success == "success") {
            std::cout << "Password changed" << std::endl;
            QMessageBox::information(new QWidget, "Erfolgreich", "Das Passwort wurde erfolgreich geändert");
            _c->_um->setCursor(Qt::WaitCursor);
            sendMessage("getUserList", QString::fromStdString(_id), "", "");
        }
    }
    else if (instruction == "endScreenshare") {
        _c->_pw->sendStrokes();
        std::cout << instruction.toStdString() << std::endl;
    }
    else if (instruction == "prepareScreenshare") {
        std::cout << arg1.toStdString() << std::endl;
        // sendMessage("saveStrokeCache", QString::fromStdString(_id), "", QByteArray::fromStdString(_c->_pw->getStrokeString().toStdString()));
        _c->_screenClient = new ScreenshareClient(new QWidget, _c, arg1);
        _c->_screenClient->showNormal();
        _c->_pres->hide();
    }
    else if (instruction == "clientCrashed") {
        std::cout << "The Client has crashed, starting App new..." << std::endl;
        _c->_pres->hide();
        _c->_hw->show();
    }
    else if (instruction == "check") {
        emit singleCheckDone(arg1, success);
    }
    else {
        std::cout << "Cannot decode message - " << instruction.toStdString() << std::endl;
    }
}

void Network::gotPreview() {
    if (_newPixes.size() == _c->_ph->_ids.size()){
        for (int i = 0; i < (int)_c->_ph->_ids.size(); i++) {
            for (int j = 0; j < (int)_newNames.size(); j++) {
                if (_c->_ph->_ids[i] == _newNames[j].toInt()) {
                    _c->_ph->_pixes.push_back(_newPixes[j]);
                }
            }
        }
        std::cout << "all" << std::endl;
        emit gotAllPreviews();
        return;
    }
    else {
        std::cout << "not all - " << _newNames.size() << std::endl;
        emit nextPreview((int)_newNames.size());
        return;
    }
}

void Network::nextPreview(int n) {
    sendMessage("getPreview", QString::fromStdString(_c->_net->_id), QString::number(_c->_ph->_ids[n]), QByteArray::fromStdString("null"));
}

bool Network::connectToHost() {
    _socket = new QTcpSocket();
    _socket->connectToHost(QString::fromStdString(_hostname), _port);
    if (_socket->waitForConnected(2000)) {
        connect(_socket, &QTcpSocket::readyRead, this, &Network::readSocket);
        return true;
    } else {
        return false;
    }
}


