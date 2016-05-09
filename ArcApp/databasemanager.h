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
    /*==========================================================================
    DATABASE MANAGER SETUP
    ==========================================================================*/
    DatabaseManager();
    bool createDatabase(QSqlDatabase* tempDbPtr, QString connName);

    /*==========================================================================
    GENERAL QUERYS
    ==========================================================================*/
    QSqlQuery selectAll(QString tableName);
    void printAll(QSqlQuery queryResults);
    QSqlQuery execQuery(QString queryString);
    bool execQuery(QSqlQuery* query, QString queryString);

    /*==========================================================================
    FILE DOWNLOAD AND UPLOAD RELATED FUNCTIONS
    ==========================================================================*/
    bool uploadCaseFile(QSqlDatabase* tempDbPtr, QString connName, QString filepath);
    bool downloadLatestCaseFile(QSqlDatabase* tempDbPtr, QString connName);
    QSqlQuery getLatestFileUploadEntry(QSqlDatabase* tempDbPtr, QString tableName);
    int getDbCounter();
    void downloadThread();
    void uploadThread(QString strFilePath);
    void printDbConnections();


    bool searchClientList(QSqlQuery* query, QString ClientId);

    QSqlQuery searchClientInfo(QSqlQuery* quer, QString ClientId);

    /*==========================================================================
    PROFILE PICTURE UPLOAD AND DOWNLOAD RELATED FUNCTIONS
    ==========================================================================*/
    bool uploadProfilePic(QSqlDatabase* tempDbPtr, QString connName, QImage profilePic);
    void uploadProfilePicThread(QString strFilePath);
    bool downloadProfilePic(QImage* img);
    bool downloadProfilePic2(QImage* img,QString idNum);
    void testuploadProfilePicThread(QString strFilePath);
    bool insertClientWithPic(QStringList* registerFieldList, QImage* profilePic);

    /*==========================================================================
    REPORT QUERYS
    ==========================================================================*/
    bool getCheckoutQuery(QSqlQuery* queryResults, QDate date);
    bool getVacancyQuery(QSqlQuery* queryResults, QDate date);
    bool getLunchQuery(QSqlQuery* queryResults, QDate date);
    bool getWakeupQuery(QSqlQuery* queryResults, QDate date);

    void print();
    QSqlQuery loginSelect(QString username, QString password);
    QSqlQuery findUser(QString username);
    QSqlQuery addNewEmployee(QString username, QString password, QString role);
    QSqlQuery updateEmployee(QString username, QString password, QString role);
    QSqlQuery deleteEmployee(QString username, QString password, QString role);
    QSqlQuery getCurrentBooking(QDate start, QDate end, QString program);
    QSqlQuery getPrograms();
    bool insertBookingTable(QString insert);
    int getMonthlyRate(QString room, QString program);
    bool addPayment(QString values);
    QSqlQuery getActiveBooking(QString user, bool userLook);
    bool downloadLatestCaseFile();
    double getRoomCost(QString roomNo);
    bool updateBooking(QString q);
    QSqlQuery pullClient(QString id);
    bool updateBalance(double d, QString id);

private:
    QSqlDatabase db = QSqlDatabase::database();
    static QMutex mutex;
    static int dbCounter;
};

extern DatabaseManager* dbManager;

#endif // DATABASEMANAGER_H
