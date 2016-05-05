#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QtSql/QtSql>
#include <stdio.h>
#include <cstdarg>
#include "shared.h"
#include "dbconfig.h"
class DatabaseManager
{
public:
    DatabaseManager();
    void print();
    QSqlQuery selectAll(QString tableName);
    QSqlQuery loginSelect(QString username, QString password);
    void printAll(QSqlQuery queryResults);

private:
    QSqlDatabase db;
};

extern DatabaseManager* dbManager;

#endif // DATABASEMANAGER_H
