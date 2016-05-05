#include "databasemanager.h"

DatabaseManager::DatabaseManager()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(DB_DRIVER);

    db.setConnectOptions();

    QString dsn =
            QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;UID=%3;PWD=%4;").
            arg(SERVER_NAME).arg(DB_NAME).arg(DB_USERNAME).arg(DB_PW);

    db.setDatabaseName(dsn);

    if (!db.open())
    {
        QString error = db.lastError().text();
        qDebug() <<error.toLocal8Bit().data();
        qDebug() << "failed to connect";
        //A pop up should alert user that there is no db connection (Maybe close the app)
    }
    else
    {
        this->db = db;
        qDebug() << "connected";
    }
}

QSqlQuery DatabaseManager::selectAll(QString tableName)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM " + tableName);
    return query;
}
QSqlQuery DatabaseManager::getPrograms(){
    QSqlQuery query(db);
    QString q = "SELECT DISTINCT ProgramCodes from Space";
    query.exec(q);
    return query;
}
int DatabaseManager::getMonthlyRate(QString room, QString program){
    QSqlQuery query(db);
    QString q = "SELECT cost FROM Space WHERE SpaceId = '" + room + "'";
    if(query.exec(q)){
        return query.value(0).toInt();
    }
    else{
        return -1;
    }
}

QSqlQuery DatabaseManager::getCurrentBooking(QDate start, QDate end, QString program){
    QSqlQuery query(db);
    QString q = "SELECT Space.SpaceId, Space.Location, Space.ProgramCodes, Space.type, Space.cost, Space.Monthly FROM Space"
                " LEFT JOIN (SELECT * from Booking WHERE Date >= '" + start.toString(Qt::ISODate) + "' AND Date <= '"
                + end.toString(Qt::ISODate) + "') AS a on Space.SpaceId = a.SpaceId WHERE BookingID IS NULL AND Space.ProgramCodes = '" + program + "'";

    query.exec(q);
    return query;
}
bool DatabaseManager::insertBookingTable(QString insert){
    QSqlQuery query(db);
    QString q = "INSERT INTO Booking VALUES(" + insert + ")";
    qDebug() << q;
    if(!query.exec(q)){
        qDebug() << "INSERT FAILED";
        return false;
    }
    return true;
}

void DatabaseManager::printAll(QSqlQuery queryResults)
{
    int numCols = queryResults.record().count();

    while (queryResults.next()) {
        QString record = "";
        for (int i = 0; i < numCols; ++i)
        {
            record += queryResults.value(i).toString() + " ";
        }
        qDebug() << record;
    }
}

