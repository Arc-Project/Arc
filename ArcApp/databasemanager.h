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
    void printAll(QSqlQuery queryResults);
    QSqlQuery getCurrentBooking(QDate start, QDate end, QString program);
    QSqlQuery getPrograms();
    bool insertBookingTable(QString insert);
    int getMonthlyRate(QString room, QString program);
private:
    QSqlDatabase db;
};

extern DatabaseManager* dbManager;

#endif // DATABASEMANAGER_H
