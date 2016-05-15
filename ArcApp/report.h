#ifndef REPORT_H
#define REPORT_H

#include <QTableView>
#include <QtSql/QtSql>
#include "mymodel.h"
#include "databasemanager.h"
#include <QtConcurrent/QtConcurrent>
#include <qDebug>

// Do not use shiftNo
#define CHECKOUT_REPORT       0
#define VACANCY_REPORT        1
#define LUNCH_REPORT          2
#define WAKEUP_REPORT         3

// Use shiftNo
#define BOOKING_REPORT        4
#define TRANSACTION_REPORT    5
#define CLIENT_REPORT         6
#define OTHER_REPORT          7

#define NUMCOLS_TRANSACTION   11
            
class Report
{
public:
    Report(QObject *parent, QTableView* tableView, int type);
    void updateModel(QDate date);
    void updateModel(QDate date, int shiftNo);
    void updateModelThread(QDate date);
private:
    MyModel model;
    int reportType;
    void updateModelThread(QDate date, int shiftNo);
    void setTransactionData(QSqlQuery* query);
    void setData(QSqlQuery* query);
    void setTitle();
};
    
#endif // REPORT_H
