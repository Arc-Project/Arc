#include "databasemanager.h"
#include "shared.h"


int DatabaseManager::dbCounter = 0;
QMutex DatabaseManager::mutex;

DatabaseManager::DatabaseManager()
{
    if (DatabaseManager::createDatabase(&db, DEFAULT_SQL_CONN_NAME))
    {
        qDebug() << "connected";
    }
    else
    {
        qDebug() << "failed to connect";
        //A pop up should alert user that there is no db connection (Maybe close the app)
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
bool DatabaseManager::addPayment(QString values){
    QSqlQuery query(db);
    QString q = "INSERT INTO Transac Values( " + values + ")";
    qDebug() << q;
    if(query.exec(q)){
        return true;
    }
    return false;
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



QSqlQuery DatabaseManager::loginSelect(QString username, QString password) {
    QSqlQuery query(db);
    query.exec("SELECT TOP 1 Role FROM Employee WHERE Username='" + username + "' AND Password='" + password + "'");
    return query;
}

bool DatabaseManager::createDatabase(QSqlDatabase* tempDbPtr, QString connName)
{
    *tempDbPtr = QSqlDatabase::addDatabase(DB_DRIVER, connName);

    tempDbPtr->setConnectOptions();

    QString dsn =
            QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;UID=%3;PWD=%4;").
            arg(SERVER_NAME).arg(DB_NAME).arg(DB_USERNAME).arg(DB_PW);

    tempDbPtr->setDatabaseName(dsn);

    if (!tempDbPtr->open())
    {
        QString error = tempDbPtr->lastError().text();
        qDebug() << error.toLocal8Bit().data();
        
        return false;
    }

    DatabaseManager::printDbConnections();

    return true;
}

void DatabaseManager::printDbConnections()
{
    QStringList list = db.connectionNames();
    for (QStringList::iterator it = list.begin(); it != list.end(); ++it) 
    {
        QString current = *it;
        qDebug() << "[[" << current << "]]";
    }
}

QSqlQuery DatabaseManager::findUser(QString username) {
    QSqlQuery query(db);
    query.exec("SELECT Username FROM Employee WHERE Username='" + username + "'");
    return query;
}

QSqlQuery DatabaseManager::addNewEmployee(QString username, QString password, QString role) {
    QSqlQuery query(db);

    query.exec("INSERT INTO Employee VALUES ('" + username + "', '" + password + "', '" + role + "')");

    return query;
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

bool DatabaseManager::uploadCaseFile(QSqlDatabase* tempDbPtr, QString connName, QString filepath)
{
    if (!DatabaseManager::createDatabase(tempDbPtr, connName))
    {
        qDebug() << "failed to create connection: " << connName;
        return false;
    }

   QByteArray byte;
   QFile file(filepath);
   if(!file.open(QIODevice::ReadOnly))
   {
       qDebug("failed to open file");
       return false;
   }

   QFileInfo fileInfo(file);
   qDebug() << "Path:\t\t\t" << fileInfo.path();
   qDebug() << "Filename w/ extension:\t" <<fileInfo.fileName();
   qDebug() << "Filename:\t\t" << fileInfo.baseName();
   qDebug() << "Extension:\t\t" << fileInfo.suffix();
   qDebug() << "Size:\t\t\t" << fileInfo.size();
   byte = file.readAll();
   file.close();

   QSqlQuery query(*tempDbPtr);
   query.prepare("INSERT INTO FileTest1(doc, fileName, extension, fileSize) VALUES(:doc, :fname, :fext, :fsize)");
   query.bindValue(":doc", byte, QSql::In | QSql::Binary);
   query.bindValue(":fname", fileInfo.fileName());
   query.bindValue(":fext", fileInfo.suffix());
   query.bindValue(":fsize", fileInfo.size());

   if (query.exec())
   {
       return true;
   }
   return false;
}

bool DatabaseManager::downloadLatestCaseFile(QSqlDatabase* tempDbPtr, QString connName)
{
    if (!DatabaseManager::createDatabase(tempDbPtr, connName))
    {
        qDebug() << "failed to create db connection: " << connName;
        return false;
    }

    QSqlQuery queryResults = DatabaseManager::getLatestFileUploadEntry(tempDbPtr, "FileTest1");
    queryResults.next();
    QString filename = queryResults.value(3).toString();
    QByteArray data = queryResults.value(2).toByteArray();
    qDebug() << filename;

    QFile file("../Downloads/" + filename);

    QFileInfo fileInfo(file);

    if (file.open(QIODevice::WriteOnly))
    {
        file.write(data);
        file.close();
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        return true;
    }
    return false;
}

QSqlQuery DatabaseManager::execQuery(QString queryString)
{
    QSqlQuery query(db);
    query.exec(queryString);
    return query;
}

QSqlQuery DatabaseManager::getLatestFileUploadEntry(QSqlDatabase* tempDbPtr, QString tableName)
{
    QSqlQuery query(*tempDbPtr);
    query.exec("SELECT TOP 1 * FROM " + tableName + " ORDER BY id DESC");
    return query;
}

int DatabaseManager::getDbCounter()
{
    int dbCounter;

    DatabaseManager::mutex.lock();
    dbCounter = DatabaseManager::dbCounter++;
    DatabaseManager::mutex.unlock();

    return dbCounter;
}

void DatabaseManager::downloadThread()
{
    QSqlDatabase tempDb = QSqlDatabase::database();
    
    QString dbConnName = QString::number(DatabaseManager::getDbCounter());

    if (dbManager->downloadLatestCaseFile(&tempDb, dbConnName))
    {
        qDebug() << "file downloaded";
    }
    else
    {
        qDebug() << "could not download file";
    }
    tempDb.close();
    QSqlDatabase::removeDatabase(dbConnName);
}

void DatabaseManager::uploadThread(QString strFilePath)
{
    QSqlDatabase tempDb = QSqlDatabase::database();

    QString dbConnName = QString::number(DatabaseManager::getDbCounter());

    if (dbManager->uploadCaseFile(&tempDb, dbConnName, strFilePath))
    {
        qDebug() << "Case file uploaded";
    }
    else
    {
        qDebug() << "Could not upload case file";
    }
    tempDb.close();
    QSqlDatabase::removeDatabase(dbConnName);
}
