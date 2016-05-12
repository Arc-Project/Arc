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
          ret = dbManager->getCheckoutQuery(&query, date);
          break;
        case VACANCY_REPORT:
          ret = dbManager->getVacancyQuery(&query, date);
          break;
        case LUNCH_REPORT:
          ret = dbManager->getLunchQuery(&query, date);
          break;
        case WAKEUP_REPORT:
          ret = dbManager->getWakeupQuery(&query, date);
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
