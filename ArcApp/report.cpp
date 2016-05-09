#include "report.h"

Report::Report(QObject *parent, QTableView* tableView, int type) :
  model(parent),
  reportType(type)
{  
    qDebug() << "report constructor";
    tableView->setModel(&model);
    tableView->show();
}

Report::updateModel(QDate date)
{
  QtConcurrent::run(this, &updateModelThread, date);
}

Report::updateModelThread(QDate date)
{
  QSqlDatabase tempDb = QSqlDatabase::database();
  QString connName = QString::number(dbManager->getDbCounter());

  if (dbManager->createDatabase(&tempDb, connName))
  {
    QSqlQuery query;
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
    QSqlDatabase::removeDatabase(connName);
  }
}

Report::setData(QSqlQuery* query)
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
