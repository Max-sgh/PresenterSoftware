#include <QtTest>
#include <QDate>
#include <iostream>
#include "tcpserver.h"
#include "room.h"

class testTcpServer: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testOpenDB();
    void testRoom();
    void testPrepareImgFailed();
    void testGetPresentationString();
};

void testTcpServer::initTestCase() {
}

void testTcpServer::cleanupTestCase() {
}

void testTcpServer::testOpenDB() {
    TcpServer* s = new TcpServer();
    s->loadDatabases();
    QCOMPARE(QString::fromStdString(s->_clients->getFilename()), QString("Data/clients.cdb"));
    //QCOMPARE(QString::fromStdString(s->_user->getFilename()), QString("Data/users.cdb"));
    //QCOMPARE(QString::fromStdString(s->_presentations->getFilename()), QString("Data/presentations.cdb"));
}

void testTcpServer::testRoom(){
    QString testUsername = "TestClassUser";
    TcpServer* s= new TcpServer();
    s->loadDatabases();
    QString ret = s->createRoom(testUsername);
    QCOMPARE(s->_rooms.size(), 1);
    Room* r = s->_rooms[0];
    QCOMPARE(r->_username, testUsername.toStdString());

    r = s->getRoom(ret);
    QCOMPARE(r->_username, testUsername.toStdString());

    QVERIFY(s->setPresentation(ret.toStdString(), "Situation des deutschen Theaters im 18. Jh."));
    QCOMPARE(r->_maxSlides, 11);
    QCOMPARE(r->_presentation, "Situation des deutschen Theaters im 18. Jh.");
    QCOMPARE(r->_currentSlide, 1);

    QVERIFY(!r->_isConnected);
}

void testTcpServer::testPrepareImgFailed() {
    TcpServer* s = new TcpServer();
    QCOMPARE(s->prepareSlide("gibtsnicht.fnoejf").toStdString(), QString("failed").toStdString());
}

void testTcpServer::testGetPresentationString() {
    TcpServer* s = new TcpServer();
    s->loadDatabases();
    QCOMPARE(s->getPresentationString("test"), QString("Test,2;TestMoin,3"));
    QCOMPARE(s->getPresentationString("gegegdfodgoef"), QString("no presentations"));
}

QTEST_GUILESS_MAIN(testTcpServer)
#include "testTcpServer.moc"
