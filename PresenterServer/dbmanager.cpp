#include "dbmanager.h"
#include <iostream>
#include <QVariant>

DbManager::DbManager(QObject *parent)
    : QObject{parent}
{
    if (!QSqlDatabase::isDriverAvailable(_driver)) {
        std::cerr << "Error! Database driver could not be load." << std::endl;
        exit(1);
    }
    QSqlDatabase db = QSqlDatabase::addDatabase(_driver);
    db.setDatabaseName(_filename);
    if (!db.open()) {
        std::cerr << "Can´t open DB!" << std::endl;
        exit(1);
    }
    db.close();

    /*QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, firstname, lastname, password, admin) VALUES ('Test2', 'Test2', 'Test2', 'Test2', 'true')");
    if (!query.exec()) {
        std::cerr << "Error: " << query.lastError().text().toStdString() << std::endl;
        exit(1);
    }
    while (query.next()) {
        std::cout << query.value("admin").toString().toStdString() << std::endl;
    }*/
}

QSqlDatabase DbManager::getDb()
{
    QSqlDatabase ret = QSqlDatabase::addDatabase(_driver);
    ret.setDatabaseName(_filename);
    if (!ret.open()) {
        std::cerr << "Can´t open DB!: " << ret.lastError().text().toStdString() << std::endl;
        exit(1);
    }
    return ret;
}

QList<QVariant> *DbManager::singleSelect(QString selectString)
{
    // Definition: ret[0] = error (bool); ret[1] = errorString/numResults
    QList<QVariant>* ret = new QList<QVariant>();
    QSqlDatabase db = getDb();
    QSqlQuery qry = db.exec(selectString);
    int results = 0;
    if (!qry.lastError().isValid()) {
        while (qry.next()) {
            results++;
        }
        ret->append(true);
        ret->append(results);
    } else {
        std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
        ret->append(false);
        ret->append(qry.lastError().text());
        return ret;
    }
    qry.first();
    do {
        ret->append(qry.value(0));
    } while(qry.next());
    db.close();
    return ret;
}

QList<QList<QVariant> *> *DbManager::multipleSelect(QString selectString, int numReturns) // numReturns = number of QVariants after execution
{
    // Definition: < <error; errorString/numResults>; <...>
    QList<QList<QVariant>*>* ret = new QList<QList<QVariant>*>();
    QSqlDatabase db = getDb();
    QSqlQuery qry = db.exec(selectString);
    int results = 0;
    if (!qry.lastError().isValid()) {
        while (qry.next()) {
            results++;
        }
        QList<QVariant>* list = new QList<QVariant>();
        list->append(true);
        list->append(results);
        ret->append(list);
    } else {
        std::cout << "Error! " << qry.lastError().text().toStdString() << std::endl;
        QList<QVariant>* list = new QList<QVariant>();
        list->append(false);
        list->append(qry.lastError().text());
        ret->append(list);
        return ret;
    }
    qry.first();
    do {
        QList<QVariant>* list = new QList<QVariant>();
        for (int i = 0; i < numReturns; i++) {
            list->append(qry.value(i));
        }
        ret->append(list);
    }while(qry.next());
    /*for (QList<QVariant>* list : *ret) {
        std::cout << list->at(0).toInt() << " " << list->at(1).toString().toStdString() << std::endl;
    }*/
    db.close();
    return ret;
}

int DbManager::update(QString updateString)
{
    QSqlDatabase db = getDb();
    QSqlQuery qry = db.exec(updateString);
    if (qry.lastError().isValid() || qry.numRowsAffected() == -1) {
        std::cerr << "DB-Error: " << qry.lastError().text().toStdString() << std::endl;
        return -1;
    }
    return qry.numRowsAffected();
}

int DbManager::insert(QString insertString)
{
    QSqlDatabase db = getDb();
    QSqlQuery qry = db.exec(insertString);
    if (qry.lastError().isValid() || qry.numRowsAffected() == -1) {
        std::cerr << "DB-Error: " << qry.lastError().text().toStdString() << std::endl;
        return -1;
    }
    return qry.numRowsAffected();
}

int DbManager::remove(QString deleteString)
{
    QSqlDatabase db = getDb();
    QSqlQuery qry = db.exec(deleteString);
    if (qry.lastError().isValid() || qry.numRowsAffected() == -1) {
        std::cerr << "DB-Error: " << qry.lastError().text().toStdString() << std::endl;
        return -1;
    }
    return qry.numRowsAffected();
}
