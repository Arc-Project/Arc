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
      *title << "Client" << "Transaction" << "Type" << "MSDD" << "Cheque #" 
             << "Cheque Date" << "Status" << "Deleted" << "Employee" << "Time"
             << "Notes";
      break;
    case CLIENT_REPORT:
      *title << "Time" << "Employee" << "Action" << "Client"; 
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
            Report::setTransactionData(&query);
          }
          break;
        case CLIENT_REPORT:
          ret = dbManager->getShiftReportClientLogQuery(&query, date, shiftNo);
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

void Report::setTransactionData(QSqlQuery* query)
{
  int numQueryCols = query->record().count();
  int numRows = 0;

  QStringList* data = new QStringList();
  while (query->next())
  {
    numRows++;
    QString fullName = QString();
    for (int i = 0; i < 3; i++)
    {
        if (!query->value(i).toString().isEmpty())
        {
            fullName.append(query->value(i).toString() + " ");
        }
    }
    *data << fullName;

    for (int i = 3; i < numQueryCols; i++)
    {
      if (i == 8 || i == 9)
      {
        if (query->value(i).toString() == "0")
        {
          if (i == 8)
            *data << "Completed";
          else
            *data << "";
        }
        else
        {
          if (i == 8)
            *data << "Pending";  
          else
            *data << "YES";
        }
      }
      else
        *data << query->value(i).toString();
    }
  }
  model.setData(data, numRows, NUMCOLS_TRANSACTION);
}

void Report::setData(QSqlQuery* query)
{
    qDebug() << reportType;
    int numCols = query->record().count();
    int numRows = 0;

    QStringList *data = new QStringList();
    while (query->next()) 
    {
      numRows++;
      for (int i = 0; i < numCols; ++i)
      {
        *data << query->value(i).toString();
      }
    }
    model.setData(data, numRows, numCols);
}
