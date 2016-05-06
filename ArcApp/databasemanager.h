#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QtSql/QtSql>
#include <stdio.h>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include "shared.h"
#include "dbconfig.h"
#include <QDesktopServices>
#include <QStringList>
#include <QMutex>

class DatabaseManager
{
public:
    DatabaseManager();
    void print();
    QSqlQuery selectAll(QString tableName);
    QSqlQuery loginSelect(QString username, QString password);
    void printAll(QSqlQuery queryResults);
    QSqlQuery getLatestFileUploadEntry(QSqlDatabase* tempDbPtr, QString tableName);
    bool uploadCaseFile(QSqlDatabase* tempDbPtr, QString connName, QString filepath);
    bool downloadLatestCaseFile(QSqlDatabase* tempDbPtr, QString connName);
    bool createDatabase(QSqlDatabase* tempDbPtr, QString connName);
    void printDbConnections();
    int getDbCounter();
    void downloadThread();
    void uploadThread(QString strFilePath);
    static QMutex mutex;  
    static int dbCounter;
private:
    QSqlDatabase db = QSqlDatabase::database();
};

extern DatabaseManager* dbManager;

#endif // DATABASEMANAGER_H
