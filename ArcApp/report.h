#ifndef REPORT_H
#define REPORT_H

#include <QTableView>
#include <QtSql/QtSql>
#include "mymodel.h"
#include "databasemanager.h"
#include <QtConcurrent/QtConcurrent>
#include <qDebug>

#define CHECKOUT_REPORT   0
#define VACANCY_REPORT    1
#define LUNCH_REPORT      2
#define WAKEUP_REPORT     3
            
class Report
{
public:
    Report(QObject *parent, QTableView* tableView, int type);
    updateModel(QDate date);
    
private:
    MyModel model;
    int reportType;
    updateModelThread(QDate date);
    setData(QSqlQuery* query);
};
    
#endif // REPORT_H
