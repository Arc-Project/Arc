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

void Report::updateModel()
{
  QtConcurrent::run(this, &updateModelThread); 
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
             << "Program" << "ESP Days" << "Balance";//"Reason" << "ESP Days" << "Balance";
      break;
    case VACANCY_REPORT:
      *title << "Space #" << "Program Codes";
      break;
    case LUNCH_REPORT:
      *title << "Client" << "Space #" << "# Lunches";
      break;
    case WAKEUP_REPORT:
      *title << "Client" << "Space #" << "Time";
      break;
    case BOOKING_REPORT:
      *title << "Client" << "Space #" << "Program" << "Start Date" 
             << "End Date" << "Action" << "Employee" << "Time";
      break;
    case TRANSACTION_REPORT:
      *title << "Client" << "Transaction" << "Type" << "Amount" << "MSDD" << "Cheque #" 
             << "Cheque Date" << "Status" << "Deleted" << "Employee" << "Time"
             << "Notes";
      break;
    case CLIENT_REPORT:
      *title << "Client" << "Action" << "Employee" << "Time";
      break;
    case OTHER_REPORT:
      *title << "Time" << "Employee" << "Log";
      break;
    case YELLOW_REPORT:
      *title << "Restriction: Yellow";
      break;
    case RED_REPORT:
      *title << "Restriction: Red";
  }
  model.setTitle(title);
}

void Report::updateModelThread()
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
        case YELLOW_REPORT:
          ret = dbManager->getYellowRestrictionQuery(&query);
          break;
        case RED_REPORT:
          ret = dbManager->getRedRestrictionQuery(&query);
          break;
      }
      if (ret)
      {
        Report::setData(&query);
      }
      tempDb.close();
    }
    else
    {
      tempDb.close();
      emit dbManager->noDatabaseConnection();
    }   
  } // Necessary braces: tempDb and query are destroyed because out of scope
  QSqlDatabase::removeDatabase(connName);
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
    else
    {
      tempDb.close();
      emit dbManager->noDatabaseConnection();
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
          break;
        case OTHER_REPORT:
          ret = dbManager->getShiftReportOtherQuery(&query, date, shiftNo); 
      }
      if (ret)
      {
        Report::setData(&query);
      }
      tempDb.close();
    }
    else
    {
      tempDb.close();
      emit dbManager->noDatabaseConnection();
    }   
  } // Necessary braces: tempDb and query are destroyed because out of scope
  QSqlDatabase::removeDatabase(connName);
}

void Report::setTransactionData(QSqlQuery* query)
{
  if (!query->isActive())
  {
    return;
  }
  int numQueryCols = query->record().count();
  qDebug() << "numQuerycols" << numQueryCols;
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
      if (i == 5)
      {
        QString balance = QString("%1%2").arg(query->value(i).toDouble() >= 0 ? "$" : "-$").
                    arg(QString::number(fabs(query->value(i).toDouble()), 'f', 2));
        *data << balance;
      }
      else if (i == 9 || i == 10)
      {
        if (query->value(i).toString() == "0")
        {
          if (i == 9)
            *data << "Completed";
          else
            *data << "";
        }
        else
        {
          if (i == 9)
            *data << "Pending";  
          else
            *data << "Yes";
        }
      }
      else
        *data << query->value(i).toString();
    }
  }
  model.setData(data, numRows, NUMCOLS_TRANSACTION, reportType);
}

void Report::setData(QSqlQuery* query)
{
    if (query->isActive())
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
        model.setData(data, numRows, numCols, reportType);
    }
}
