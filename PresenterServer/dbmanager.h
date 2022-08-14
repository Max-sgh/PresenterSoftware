#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QList>
#include <QVariant>

class DbManager : public QObject
{
    Q_OBJECT
public:
    explicit DbManager(QObject *parent = nullptr);
    const QString _driver = "QSQLITE";
    const QString _filename = "Data/Database.db";
    QSqlDatabase getDb();
    QList<QVariant>* singleSelect(QString selectString);
    QList<QList<QVariant>*>* multipleSelect(QString selectString, int numReturns);
    int update(QString updateString);
    int insert(QString insertString);
    int remove(QString deleteString);

signals:

};

#endif // DBMANAGER_H
