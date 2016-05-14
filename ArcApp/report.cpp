#include "report.h"

Report::Report(QObject *parent, QTableView* tableView, int type) :
  model(parent),
  reportType(type)
{  
    qDebug() << "report constructor";
    Report::setTitle();
    tableView->setModel(&model);
    tableView->show();
}

void Report::updateModel(QDate date)
{
  QtConcurrent::run(this, &updateModelThread, date);
}

void Report::updateModel(QDate date, const int shiftNo)
{
  QtConcurrent::run(this, &updateModelThread, date, shiftNo);
}

void Report::setTitle()
{
  QStringList* title = new QStringList();
  switch(reportType)
  {
    case CHECKOUT_REPORT:
      *title << "Client" << "Space #" << "Start Date" << "End Date" 
             << "Program" << "Reason" << "ESP Days" << "Balance";
      break;
    case VACANCY_REPORT:
      *title << "Space #" << "Program Codes";
      break;
    case LUNCH_REPORT:
      *title << "Client" << "Space #" << "Lunch";
      break;
    case WAKEUP_REPORT:
      *title << "Client" << "Space #" << "Time";
      break;
    case BOOKING_REPORT:
      *title << "Client" << "Space #" << "Program" << "Start Date" 
             << "End Date" << "Action" << "Status" << "Employee" << "Time";
      break;
    case TRANSACTION_REPORT:
      *title << "Client" << "Transaction Type" << "Type" << "MSDD" << "Cheque #" 
             << "Cheque Date" << "Status" << "Deleted" << "Employee" << "Time"
             << "Notes" << "12" << "13";
  }
  model.setTitle(title);
}

void Report::updateModelThread(QDate date)
{
  QString connName = QString::number(dbManager->getDbCounter());
  {
    QSqlDatabase tempDb = QSqlDatabase::database();

    if (dbManager->createDatabase(&tempDb, connName))
    {
      QSqlQuery query(tempDb);
      bool ret = false;
      switch(reportType)
      {
        case CHECKOUT_REPORT:
          ret = dbManager->getDailyReportCheckoutQuery(&query, date);
          break;
        case VACANCY_REPORT:
          ret = dbManager->getDailyReportVacancyQuery(&query, date);
          break;
        case LUNCH_REPORT:
          ret = dbManager->getDailyReportLunchQuery(&query, date);
          break;
        case WAKEUP_REPORT:
          ret = dbManager->getDailyReportWakeupQuery(&query, date);
          break;
        
      }
      if (ret)
      {
        Report::setData(&query);
      }
      tempDb.close();
    }   
  } // Necessary braces: tempDb and query are destroyed because out of scope
  QSqlDatabase::removeDatabase(connName);
}

void Report::updateModelThread(QDate date, int shiftNo)
{
  QString connName = QString::number(dbManager->getDbCounter());
  {
    QSqlDatabase tempDb = QSqlDatabase::database();

    if (dbManager->createDatabase(&tempDb, connName))
    {
      QSqlQuery query(tempDb);
      bool ret = false;
      switch(reportType)
      {
        case BOOKING_REPORT:
          ret = dbManager->getShiftReportBookingQuery(&query, date, shiftNo);
          break;
        case TRANSACTION_REPORT:
          if (dbManager->getShiftReportTransactionQuery(&query, date, shiftNo))
          {
            ret = true;
          }
      }
      if (ret)
      {
        Report::setData(&query);
      }
      tempDb.close();
    }   
  } // Necessary braces: tempDb and query are destroyed because out of scope
  QSqlDatabase::removeDatabase(connName);
}

bool Report::processTransactionQuery(QStringList* data, QSqlQuery* query)
{
  return true;
}

void Report::setData(QSqlQuery* query)
{
    qDebug() << reportType;
    int numCols = query->record().count();
    int numRows = 0;

    QStringList *data = new QStringList();
    while (query->next()) {
        numRows++;
        for (int i = 0; i < numCols; ++i)
        {
          *data << query->value(i).toString();
        }
    }
    model.setData(data, numRows, numCols);
}

void Report::setData(QStringList *data)
{
  int numRows = 0, numCols = 0;
  model.setData(data, numRows, numCols);
}
