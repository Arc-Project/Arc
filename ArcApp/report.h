#ifndef REPORT_H
#define REPORT_H

#include <QTableView>
#include <QtSql/QtSql>
#include "mymodel.h"
#include "databasemanager.h"
#include <QtConcurrent/QtConcurrent>
#include <qDebug>
#include "shared.h"

            
class Report
{
public:
    Report(QObject *parent, QTableView* tableView, int type);
    void updateModel();
    void updateModel(QDate date);
    void updateModel(QDate date, int shiftNo);
    void updateModelThread();
    void updateModelThread(QDate date);
    void updateModelThread(QDate date, int shiftNo);
    MyModel model;
private:
    int reportType;
    void setTransactionData(QSqlQuery* query);
    void setData(QSqlQuery* query);
    void setTitle();
};
    
#endif // REPORT_H
