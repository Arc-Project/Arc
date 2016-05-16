#include "databasemanager.h"
#include "shared.h"


int DatabaseManager::dbCounter = 0;
QMutex DatabaseManager::mutex;

//ini file stuff
//QSettings settings;
//QString SERVER_NAME;
//QString DB_NAME;
//QString DB_USERNAME;
//QString DB_PW;

/*==============================================================================
DATABASE MANAGER SETUP (START)
==============================================================================*/
/*
 * Constuctor
 * Sets up the default database connection.
 */
DatabaseManager::DatabaseManager(QObject *parent) :
    QObject(parent)
{
   qRegisterMetaType< IntList >( "IntList" );

   //ini file stuff
//   QSettings settings(QSettings::IniFormat, QSettings::UserScope,
//                      "The Salvation Army", "ARCWay");
//   qDebug() << "ini path" << settings.fileName();
//   settings.beginGroup("database");
//   qDebug() << "server name: " << settings.value("SERVER_NAME").toString();
//   //hack to make this work with this particular server name that has backslashes and commas
//   QVariant value = settings.value("SERVER_NAME2");
//   QString server_name2;
//   if (value.type() == QVariant::StringList) {
//     server_name2 = value.toStringList().join(",");
//   } else {
//     server_name2 = value.toString();
//   }
//   qDebug() << "server name part 2: " << server_name2;
//   qDebug() << "db name: " << settings.value("DB_NAME").toString();
//   qDebug() << "db username: " << settings.value("DB_USERNAME").toString();
//   qDebug() << "db pw: " << settings.value("DB_PW").toString();

//   //set constants

//   SERVER_NAME = settings.value("SERVER_NAME").toString() + "\\" + server_name2;
//   DB_NAME = settings.value("DB_NAME").toString();
//   DB_USERNAME = settings.value("DB_USERNAME").toString();
//   DB_PW = settings.value("DB_PW").toString();

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

/*
 * Creates and adds a new database connection to the DatabaseManager.
 * Multiple connections may be required to performing querys asynchronously.
 *
 * @PARAM: QSqlDatabase* tempDbPtr: A pointer to the QSqlDatabase object
 * @PARAM: QString connName: The name of the connection
 *
 * @RETURN: bool: true if database connection created succesfully, false otherwise
 */
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
/*==============================================================================
DATABASE MANAGER SETUP (END)
==============================================================================*/

/*==============================================================================
GENERAL QUERYS (START)
==============================================================================*/
/*
 * Selects all rows from the specified table.
 *
 * @PARAM: QString tableName: The name of the sql server table
 *
 * @RETURN: QSqlQuery; The QSqlQuery object containing all the rows of the table
 */
QSqlQuery DatabaseManager::selectAll(QString tableName)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM " + tableName);
    return query;
}

/*
 * Prints all the results of QSqlQuery object.
 *
 * @PARAM: QSqlQuery queryResults: The object containing the query results
 *
 * @RETURN: void
 */
void DatabaseManager::printAll(QSqlQuery queryResults)
{
    int numCols = queryResults.record().count();

    while (queryResults.next()) {
        QString record = "";
        for (int i = 0; i < numCols; ++i)
        {
           //qDebug() << i << " : " << queryResults.value(i).toString();
             record += queryResults.value(i).toString() + " ";
        }
        qDebug() << record;
    }
}

QSqlQuery DatabaseManager::execQuery(QString queryString)
{
    qDebug()<<"execQuery. ";
    QSqlQuery query(db);
    query.exec(queryString);
    return query;
}

bool DatabaseManager::execQuery(QSqlQuery* query, QString queryString)
{
    return query->exec(queryString);
}
/*==============================================================================
GENERAL QUERYS (END)
==============================================================================*/

/*==============================================================================
FILE DOWLOAD AND UPLOAD RELATED FUNCTIONS (START)
==============================================================================*/
/*
 * Uploads a file to the database.
 *
 * @PARAM: QSqlDatabase* tempDbPtr: The database connection
 * @PARAM: QString connName: The name of the connection (unique);
 * @PARAM: QString filepath: The local file path of the file to upload
 *
 * @RETURN: bool: true if upload succeeds, false otherwise
 */
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

/*
 * Download the latest uploaded file from the database.
 *
 * @PARAM: QSqlDatabase* tempDbPtr: The database connection
 * @PARAM: QString connName: The name of the connection (unique);
 *
 * @RETURN: bool: true if download succeeds, false otherwise
 */
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

/*
 * Retrieves the QSqlQuery object containing the last added row of the table specified
 *
 * @PARAM: QSqlDatabase* tempDbPtr: The database connection
 * @PARAM: QString table: The database table name;
 *
 * @RETURN: QSqlQuery: The QSqlQuery object containing the last row of the table specified
 */
QSqlQuery DatabaseManager::getLatestFileUploadEntry(QSqlDatabase* tempDbPtr, QString tableName)
{
    QSqlQuery query(*tempDbPtr);
    query.exec("SELECT TOP 1 * FROM " + tableName + " ORDER BY id DESC");
    return query;
}

/*
 * Retrives the current database connection counter and increments it.
 *
 * @RETURN: int: The current database connection counter
 */
int DatabaseManager::getDbCounter()
{
    int dbCounter;

    DatabaseManager::mutex.lock();
    dbCounter = DatabaseManager::dbCounter++;
    DatabaseManager::mutex.unlock();

    return dbCounter;
}

/*
 * Thread function for downloading files from the database.
 *
 * @RETURN: void
 */
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

/*
 * Thread function for uploading files into the database.
 *
 * @PARAM: QString strFilePath: The local path of the file to upload
 *
 * @RETURN: void
 */
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

/*
 * Prints the current active database connections
 *
 * @RETURN: void
 */
void DatabaseManager::printDbConnections()
{
    QStringList list = db.connectionNames();
    for (QStringList::iterator it = list.begin(); it != list.end(); ++it)
    {
        QString current = *it;
        qDebug() << "[[" << current << "]]";
    }
}
/*==============================================================================
FILE DOWLOAD AND UPLOAD RELATED FUNCTIONS (END)
==============================================================================*/


/*==============================================================================
SEARCH CLIENT LIST FUNCTION(START)
==============================================================================*/


QSqlQuery DatabaseManager::searchClientList(QString ClientName){
    QSqlQuery query(db);

    //default select query
    QString searchQuery = QString("SELECT ClientId, FirstName, MiddleName, LastName, Dob, Balance ")
            + QString("FROM Client ");
    QStringList clientNames;
    if(ClientName != ""){
        clientNames = ClientName.split(" ");
    }
    //if 1 word name, match first name or last name
    if(clientNames.count() == 1){
        qDebug()<<"Name 1 words";
        searchQuery += QString("WHERE (FirstName LIKE '"+clientNames.at(0) + "%' ")
                     + QString("OR LastName Like '"+clientNames.at(0)+"%') ")
                     + QString("AND NOT FirstName = 'anonymous' ")
                     + QString("ORDER BY FirstName ASC, LastName ASC");
    }
    //if 2 word name, match first name and last name
    else if(clientNames.count() == 2){
        qDebug() << "Name 2 words";
        searchQuery += QString("WHERE (LastName LIKE '"+clientNames.at(0) + "%' AND FirstName LIKE '" + clientNames.at(1) + "%') ")
                     + QString("OR (FirstName Like '"+clientNames.at(0)+"%' AND LastName LIKE '" + clientNames.at(1) + "%') ")
                     + QString("AND NOT FirstName = 'anonymous' ")
                     + QString("ORDER BY FirstName ASC, LastName ASC");
    }
    else{
        qDebug()<<"no name or more than 1 ";
    }
    query.prepare(searchQuery);
    query.exec();
    return query;

}

QSqlQuery DatabaseManager::searchClientInfo(QString ClientId){
    QSqlQuery selectquery(db);

    selectquery.prepare(QString("SELECT FirstName, MiddleName, LastName, Dob, Balance, ")
                      + QString("SinNo, GaNo, EmpId, DateRulesSigned, status, ")
                      + QString("NokName, NokRelationship, NokLocation, NokContactNo, PhysName, ")
                      + QString("PhysContactNo, SuppWorker1Name, SuppWorker1ContactNo, SuppWorker2Name, SuppWorker2ContactNo, ")
                      + QString("Comments ")//, ProfilePic ")
                      + QString("FROM Client WHERE ClientId =" + ClientId));


    selectquery.exec();

  //  printAll(selectquery);
    return selectquery;
}


bool DatabaseManager::searchClientInfoPic(QImage * img, QString ClientId){
    QString connName = QString::number(DatabaseManager::getDbCounter());
    {
        QSqlDatabase tempDb = QSqlDatabase::database();
        if (DatabaseManager::createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);
            if (!query.exec("SELECT ProfilePic FROM Client WHERE ClientId =" + ClientId))
            {
                qDebug() << "downloadProfilePic query failed";
                return false;
            }
            query.next();
            QByteArray data = query.value(0).toByteArray();
            *img = QImage::fromData(data, "PNG");

            tempDb.close();

        }
        tempDb.close();
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
    return true;
}

QSqlQuery DatabaseManager::searchClientTransList(int maxNum, QString clientId){
    QSqlQuery clientTransQuery;
    clientTransQuery.prepare(QString("SELECT TOP "+ QString::number(maxNum) )
                           + QString(" t.Date, t.Amount, t.Type, e.Username, t.ChequeNo, t.ChequeDate, t.Deleted ")
                           + QString("FROM Transac t INNER JOIN Employee e ON t.EmpId = e.EmpId ")
                           + QString("WHERE ClientId = " + clientId + " ORDER BY Date DESC"));
    clientTransQuery.exec();
    return clientTransQuery;
}

QSqlQuery DatabaseManager::searchBookList(int maxNum, QString clientId){
    QSqlQuery clientBookingQuery;
    clientBookingQuery.prepare(QString("SELECT TOP "+ QString::number(maxNum) )
                           + QString(" DateCreated, ProgramCode, StartDate, EndDate, Cost, ")
                           + QString("SpaceId, FirstBook ")
                           + QString("FROM Booking ")
                           + QString("WHERE ClientId = " + clientId + " ORDER BY EndDate DESC, DateCreated DESC"));
    clientBookingQuery.exec();
    return clientBookingQuery;
}

int DatabaseManager::countInformationPerClient(QString tableName, QString ClientId){
    QSqlQuery countQuery;
    countQuery.prepare(QString("SELECT COUNT(*) FROM " + tableName )
                     + QString(" WHERE ClientId = " + ClientId));
    countQuery.exec();
    countQuery.next();
    qDebug()<<"QUERY Count " + countQuery.value(0).toString();
    return countQuery.value(0).toInt();
}

/*==============================================================================
PROFILE PICTURE UPLOAD AND DOWNLOAD RELATED FUNCTIONS (START)
==============================================================================*/
bool DatabaseManager::uploadProfilePic(QSqlDatabase* tempDbPtr, QString connName, QImage profilePic)
{
    if (!DatabaseManager::createDatabase(tempDbPtr, connName))
    {
        qDebug() << "failed to create connection: " << connName;
        return false;
    }

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    profilePic.save(&buffer, "PNG");  // writes image into ba in PNG format

    QSqlQuery query(*tempDbPtr);
    query.prepare("UPDATE Client SET ProfilePic = :profilePic WHERE ClientId = 6");
    query.bindValue(":profilePic", ba, QSql::In | QSql::Binary);

    if (query.exec())
    {
        return true;
    }
    return false;
}
bool DatabaseManager::insertIntoBookingHistory(QString clientName, QString spaceId, QString program, QString start, QString end, QString action, QString emp, QString shift){
    QSqlQuery query(db);
    QString q = "INSERT INTO BookingHistory VALUES ('" +clientName + "','" +
            spaceId + "','" + program + "','" + QDate::currentDate().toString(Qt::ISODate)
            + "','" + start + "','" + end + "','" + action + "','" + "UNKNOWN"
            + "','" + emp + "','" + shift + "','" + QTime::currentTime().toString() + "')";
    qDebug() << q;
    return query.exec(q);


}
bool DatabaseManager::addHistoryFromId(QString bookId, QString empId, QString shift, QString action){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Booking WHERE BookingId = '" + bookId + "'";
    if(!query.exec(q)){
        return false;
    }
    while(query.next()){
        QString clientName =query.value("ClientName").toString();
        QString spaceId = query.value("SpaceId").toString();
        QString program = query.value("ProgramCode").toString();
        QString start = query.value("StartDate").toString();
        QString end = query.value("EndDate").toString();
        return insertIntoBookingHistory(clientName, spaceId, program, start, end, action, empId, shift );

    }

}

bool DatabaseManager::insertClientWithPic(QStringList* registerFieldList, QImage* profilePic)//QObject sth, QImage profilePic)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    profilePic->save(&buffer, "PNG");  // writes image into ba in PNG format

    QSqlQuery query(db);

    query.prepare(QString("INSERT INTO Client ")
        + QString("(FirstName, MiddleName, LastName, Dob, SinNo, GaNo, EmpId, ")
        + QString("DateRulesSigned, NokName, NokRelationship, NokLocation, NokContactNo, ")
        + QString("PhysName, PhysContactNo, SuppWorker1Name, SuppWorker1ContactNo, ")
        + QString("SuppWorker2Name, SuppWorker2ContactNo, Status, Comments, ProfilePic) ")
        + QString("VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

    for (int i = 0; i < registerFieldList->size(); ++i)
    {
        if (registerFieldList->at(i) != NULL)
        {
            qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
            query.addBindValue(registerFieldList->at(i));
        }
        else
        {
            query.addBindValue(QVariant(QVariant::String));
        }
    }

    query.addBindValue(ba, QSql::In | QSql::Binary);

    if (query.exec())
    {
        return true;
    }
    return false;
}


bool DatabaseManager::insertClientLog(QStringList* registerFieldList)
{
    QSqlQuery query(db);

    query.prepare(QString("INSERT INTO ClientLog ")
        + QString("(ClientName, Action, Date, ShiftNo, Time, EmpName) ")
        + QString("VALUES(?, ?, ?, ?, ?, ?)"));

    for (int i = 0; i < registerFieldList->size(); ++i)
    {
        if (registerFieldList->at(i) != NULL)
        {
            qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
            query.addBindValue(registerFieldList->at(i));
        }
        else
        {
            query.addBindValue(QVariant(QVariant::String));
        }
    }
    if (query.exec())
    {
        return true;
    }
    return false;
}

bool DatabaseManager::updateClientWithPic(QStringList* registerFieldList, QString clientId, QImage* profilePic)//QObject sth, QImage profilePic)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    profilePic->save(&buffer, "PNG");  // writes image into ba in PNG format

    QSqlQuery query(db);

    query.prepare(QString("UPDATE Client ")
        + QString("SET FirstName = ? , MiddleName = ?, LastName = ?, Dob = ?, SinNo = ?, GaNo = ?, EmpId = ?, ")
        + QString("DateRulesSigned = ?, NokName = ?, NokRelationship = ?, NokLocation = ?, NokContactNo = ?, ")
        + QString("PhysName = ?, PhysContactNo = ?, SuppWorker1Name = ?, SuppWorker1ContactNo = ?, ")
        + QString("SuppWorker2Name = ?, SuppWorker2ContactNo = ?, Status = ?, Comments = ?, ProfilePic = ? ")
        + QString("WHERE ClientId = " + clientId));

    for (int i = 0; i < registerFieldList->size(); ++i)
    {
        if (registerFieldList->at(i) != NULL)
        {
            qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
            query.addBindValue(registerFieldList->at(i));
        }
        else
        {
            query.addBindValue(QVariant(QVariant::String));
        }
    }

    query.addBindValue(ba, QSql::In | QSql::Binary);

    if (query.exec())
    {
        return true;
    }
    return false;
}



QSqlQuery DatabaseManager::getTransactions(QDate start, QDate end){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Transac JOIN Client on Transac.ClientId = Client.ClientID WHERE Date >= '" + start.toString(Qt::ISODate) + "' AND Date <= '"
            + end.toString(Qt::ISODate) + "'  AND Deleted = 0";
    qDebug() << q;
    query.exec(q);
    return query;
}
QSqlQuery DatabaseManager::getOwingClients(){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Client WHERE Balance < 0";
    query.exec(q);
    return query;
}
QSqlQuery DatabaseManager::setLunches(QDate date, int num, QString id){
    QSqlQuery query(db);
    QString q = "INSERT INTO Lunches Values('" + id + "','" + date.toString(Qt::ISODate) + "','" + QString::number(num) + "')";
    qDebug() << q;
    query.exec(q);
    return query;

}
QSqlQuery DatabaseManager::getLunches(QDate start, QDate end, QString id){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Lunches WHERE ClientId = '" + id + "' AND LunchDate >= '" + start.toString(Qt::ISODate)
            + "' AND LunchDate <= '" + end.toString(Qt::ISODate) + "'";
    qDebug() << q;
    query.exec(q);
    return query;
 }
bool DatabaseManager::updateLunches(QDate date, int num, QString id){
    QSqlQuery query(db);
    QString q = "UPDATE Lunches SET Number ='" + QString::number(num) + "' WHERE LunchDate = '"
            + date.toString(Qt::ISODate) + "' AND ClientId = '" + id + "'";
    qDebug() << q;
    return query.exec(q);
}
QSqlQuery DatabaseManager::getWakeups(QDate start, QDate end, QString id){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Wakeup WHERE ClientId = '" + id + "' AND WakeDate >= '" + start.toString(Qt::ISODate)
            + "' AND WakeDate <= '" + end.toString(Qt::ISODate) + "'";
    qDebug() << q;
    query.exec(q);
    return query;
}
bool DatabaseManager::deleteWakeups(QDate date, QString id){
    QSqlQuery query(db);
    QString q = "DELETE FROM Wakeup WHERE ClientId ='" + id + "' AND WakeDate = '" + date.toString(Qt::ISODate) + "'";
    qDebug() << q;
    return query.exec(q);
}
bool DatabaseManager::deleteWakeupsMulti(QDate date, QString id){
    QSqlQuery query(db);
    QString q = "DELETE FROM Wakeup WHERE ClientId ='" + id + "' AND WakeDate >= '" + date.toString(Qt::ISODate) + "'";
    qDebug() << q;
    return query.exec(q);
}
bool DatabaseManager::updateWakeups(QDate date, QString time, QString id){
    QSqlQuery query(db);
    QString q = "UPDATE Wakeup SET WakeTime ='" + time + "' WHERE WakeDate = '"
            + date.toString(Qt::ISODate) + "' AND ClientId = '" + id + "'";
    qDebug() << q;
    return query.exec(q);
}

bool DatabaseManager::setWakeup(QDate date, QString time, QString id){
    QSqlQuery query(db);
    QString q = "INSERT INTO Wakeup Values('" + id + "','" + date.toString(Qt::ISODate) + "','" + time + "')";
    qDebug() << q;
    return query.exec(q);

}
bool DatabaseManager::removeLunchesMulti(QDate date, QString id){
    QSqlQuery query(db);
    QString q = "DELETE FROM Lunches WHERE LunchDate >= '" + date.toString(Qt::ISODate) + "' AND ClientID ='"+
            id + "'";
    qDebug() << q;
    return query.exec(q);
}
bool DatabaseManager::removeLunches(QDate date, QString id){
    QSqlQuery query(db);
    QString q = "DELETE FROM Lunches WHERE LunchDate = '" + date.toString(Qt::ISODate) + "' AND ClientID ='"+
            id + "'";
    qDebug() << q;
    return query.exec(q);
}

void DatabaseManager::uploadProfilePicThread(QString strFilePath)
{
    QSqlDatabase tempDb = QSqlDatabase::database();

    QString dbConnName = QString::number(DatabaseManager::getDbCounter());

    QImage img(strFilePath);

    if (dbManager->uploadProfilePic(&tempDb, dbConnName, img))
    {
        qDebug() << "Profile pic uploaded";
    }
    else
    {
        qDebug() << "Could not upload profile pic";
    }
    tempDb.close();
    QSqlDatabase::removeDatabase(dbConnName);
}

void DatabaseManager::testuploadProfilePicThread(QString strFilePath)
{
    QSqlDatabase tempDb = QSqlDatabase::database();

    QString dbConnName = QString::number(DatabaseManager::getDbCounter());

    QImage img(strFilePath);

    if (dbManager->uploadProfilePic(&tempDb, dbConnName, img))
    {
        qDebug() << "Profile pic uploaded";
    }
    else
    {
        qDebug() << "Could not upload profile pic";
    }
    tempDb.close();
    QSqlDatabase::removeDatabase(dbConnName);
}

bool DatabaseManager::downloadProfilePic(QImage* img)
{
    QSqlQuery query(db);
    if (!query.exec("SELECT ProfilePic FROM Client WHERE ClientId = 7"))
    {
        qDebug() << "downloadProfilePic query failed";
        return false;
    }

    query.next();
    QByteArray data = query.value(0).toByteArray();
    *img = QImage::fromData(data, "PNG");

    return true;
}

bool DatabaseManager::downloadProfilePic2(QImage* img, QString idNum)
{
    QSqlQuery query(db);
    if (!query.exec("SELECT ProfilePic FROM Client WHERE ClientId =" + idNum))
    {
        qDebug() << "downloadProfilePic query failed";
        return false;
    }

    query.next();
    if(query.value(0).isNull()){
        qDebug()<<"picture value is NULL";
        return false;
    }
    qDebug()<<"pic value is not null";
    qDebug()<<"what hold in pic field" <<query.value(0).toString();

    QByteArray data = query.value(0).toByteArray();
    *img = QImage::fromData(data, "PNG");
printAll(query);
    return true;
}

/*==============================================================================
PROFILE PICTURE UPLOAD AND DOWNLOAD RELATED FUNCTIONS (END)
==============================================================================*/
/*==============================================================================
REPORT QUERYS (START)
==============================================================================*/
bool DatabaseManager::getDailyReportCheckoutQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
        QString("SELECT b.ClientName, b.SpaceId, b.StartDate, ")
        + QString("b.EndDate, b.ProgramCode, c.Balance ")
        + QString("FROM Booking b INNER JOIN Client c ON b.ClientId = c.ClientId ")
        + QString("WHERE EndDate = '" + date.toString(Qt::ISODate))
        + QString("' AND FirstBook = 'YES'");

        // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getDailyReportVacancyQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
        QString("SELECT s.SpaceId, s.ProgramCodes ")
        + QString("FROM Space s LEFT JOIN (SELECT SpaceId, Date ")
        + QString("FROM Booking WHERE Date = '" + date.toString(Qt::ISODate))
        + QString("') as b ON s.SpaceId = b.SpaceId ")
        + QString("WHERE b.date IS NULL");

        // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getDailyReportLunchQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
        QString("SELECT ClientName, SpaceId, Lunch ")
        + QString("FROM Booking ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate)) + "' AND "
        + QString("Lunch = 'YES'");

        // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getDailyReportWakeupQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
        QString("SELECT ClientName, SpaceId, Wakeup ")
        + QString("FROM Booking ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate)) + "' AND "
        + QString("Wakeup <> 'NO' AND Wakeup IS NOT NULL");

        // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getShiftReportBookingQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString =
        QString("SELECT ClientName, SpaceCode, ProgramCode, StartDate, ")
        + QString("EndDate, Action, Status, EmpName, Time ")
        + QString("FROM BookingHistory ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo));
    // qDebug() << queryString;
    return queryResults->exec(queryString);
}
QSqlQuery DatabaseManager::getSwapBookings(QDate start, QDate end, QString program, QString maxSwap, QString curBook){
    QSqlQuery query(db);
    QString q = "SELECT * FROM (SELECT COUNT(SpaceID) C, SpaceId FROM Booking"
                " WHERE EndDate > '" + start.toString(Qt::ISODate) + "' AND StartDate <= '" + end.toString(Qt::ISODate)
            +"' AND EndDate <= '" + maxSwap +"' GROUP BY SpaceId ) AS T LEFT JOIN (SELECT * FROM Space) AS S on S.SpaceId = T.SpaceId" +
            " JOIN ( SELECT * FROM Booking WHERE EndDate > '" + start.toString(Qt::ISODate) + "' AND StartDate <= '" + end.toString(Qt::ISODate)
            +"' AND EndDate <= '" + maxSwap +"') V on T.SpaceId = V.SpaceId WHERE C = 1 AND ProgramCodes ='" + program + "' AND BookingId != '" + curBook + "'";

    query.exec(q);
    qDebug() << q;
    return query;
}

bool DatabaseManager::getShiftReportTransactionQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString =
        QString("SELECT c.FirstName, c.MiddleName, c.LastName, t.TransType, ")
        + QString("t.Type, t.MSQ, t.ChequeNo, t.ChequeDate, ")
        + QString("CAST(t.Outstanding AS INT), CAST(t.Deleted AS INT), ")
        + QString("t.EmpName, t.Time, t.Notes " )
        + QString("FROM Client c INNER JOIN Transac t ON c.ClientId = t.ClientId ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo));
    // qDebug() << queryString;
    return queryResults->exec(queryString);
}

int DatabaseManager::getDailyReportEspCheckouts(QDate date)
{
    QString queryString =
            QString("SELECT COUNT(ClientId) ")
            + QString("FROM Booking ")
            + QString("WHERE EndDate = '" + date.toString(Qt::ISODate))
            + QString("' AND FirstBook = 'YES' AND ProgramCode = 'ESP'");
    // qDebug() << queryString;
    return DatabaseManager::getIntFromQuery(queryString);
}

int DatabaseManager::getDailyReportTotalCheckouts(QDate date)
{
    QString queryString =
            QString("SELECT COUNT(ClientId) ")
            + QString("FROM Booking ")
            + QString("WHERE EndDate = '" + date.toString(Qt::ISODate))
            + QString("' AND FirstBook = 'YES'");
    // qDebug() << queryString;
    return DatabaseManager::getIntFromQuery(queryString);
}


int DatabaseManager::getDailyReportEspVacancies(QDate date)
{
    QString queryString =
            QString("SELECT COUNT(s.SpaceId) ")
            + QString("FROM SPACE s LEFT JOIN ")
            + QString("(SELECT SpaceId, Date FROM Booking WHERE Date = '")
            + QString(date.toString(Qt::ISODate) + "') as b ")
            + QString("ON s.SpaceId = b.SpaceId ")
            + QString("WHERE b.Date IS NULL AND s.ProgramCodes LIKE 'ESP'");
    // qDebug() << queryString;
    return DatabaseManager::getIntFromQuery(queryString);
}

int DatabaseManager::getDailyReportTotalVacancies(QDate date)
{
    QString queryString =
            QString("SELECT COUNT(s.SpaceId) ")
            + QString("FROM SPACE s LEFT JOIN ")
            + QString("(SELECT SpaceId, Date FROM Booking WHERE Date = '")
            + QString(date.toString(Qt::ISODate) + "') as b ")
            + QString("ON s.SpaceId = b.SpaceId ")
            + QString("WHERE b.Date IS NULL");
    // qDebug() << queryString;
    return DatabaseManager::getIntFromQuery(queryString);
}

void DatabaseManager::getDailyReportStatsThread(QDate date)
{
    QList<int> list;
    list << DatabaseManager::getDailyReportEspCheckouts(date)
         << DatabaseManager::getDailyReportTotalCheckouts(date)
         << DatabaseManager::getDailyReportEspVacancies(date)
         << DatabaseManager::getDailyReportTotalVacancies(date);
    emit DatabaseManager::dailyReportStatsChanged(list);
}

int DatabaseManager::getShiftReportTotal(QDate date, int shiftNo, QString payType)
{
    QString queryString = 
        QString("SELECT ISNULL(p.total, 0) - ISNULL(r.total, 0) ")
        + QString("FROM ")
        + QString("(SELECT SUM(Amount) as total ")
        + QString("FROM Transac ")
        + QString("WHERE (Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo) + " AND ")
        + QString("Type = '" + payType + "')")
        + QString("AND ((TransType = 'Payment' AND Deleted = 0 AND Outstanding = 0) ")
        + QString("OR (TransType = 'Refund' AND Deleted = 1 AND Outstanding = 0))) as p ")
        + QString("CROSS JOIN ")
        + QString("(SELECT SUM(Amount) as total ")
        + QString("FROM Transac ")
        + QString("WHERE (Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo) + " AND ")
        + QString("Type = '" + payType + "')")
        + QString("AND ((TransType = 'Refund' AND Deleted = 0 AND Outstanding = 0) ")
        + QString("OR (TransType = 'Payment' AND Deleted = 1 AND Outstanding = 0))) as r");
    // qDebug() << queryString;
    return DatabaseManager::getIntFromQuery(queryString);
}

void DatabaseManager::getShiftReportStatsThread(QDate date, int shiftNo)
{
    int cashTotal = DatabaseManager::getShiftReportTotal(date, shiftNo, PAY_CASH);
    int electronicTotal = DatabaseManager::getShiftReportTotal(date, shiftNo, PAY_ELECTRONIC);
    int chequeTotal = DatabaseManager::getShiftReportTotal(date, shiftNo, PAY_CHEQUE);

    QList<int> list = QList<int>() << cashTotal << electronicTotal << chequeTotal
                                   << cashTotal + chequeTotal
                                   << cashTotal + electronicTotal + chequeTotal;
    emit DatabaseManager::shiftReportStatsChanged(list);
}

bool DatabaseManager::getShiftReportClientLogQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString =
        QString("SELECT CONVERT(VARCHAR(15), Time, 100), EmpName, Action, ClientName ")
        + QString("FROM ClientLog ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo))
        + QString("ORDER BY Time Desc");
    qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getShiftReportOtherQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString = 
        QString("SELECT Time, EmpName, Log ")
        + QString("FROM OtherLog ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo))
        + QString("ORDER BY Time Desc");
    qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::insertOtherLog(QString empName, int shiftNo, QString logText)
{
    QSqlQuery query(db);

    query.prepare("INSERT INTO OtherLog (Date, ShiftNo, Time, EmpName, Log) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(QDate::currentDate().toString(Qt::ISODate));
    query.addBindValue(shiftNo);
    query.addBindValue(QTime::currentTime().toString("hh:mm:ss"));
    query.addBindValue(empName);
    query.addBindValue(logText);
    
    return query.exec();
}

bool DatabaseManager::getCashFloatReportQuery(QSqlQuery* queryResults, QDate date, int shiftNo)
{
    QString queryString = 
        QString("SELECT EmpName, DateEdited, TimeEdited, Comments, Nickels, Dimes, ")
        + QString("Quarters, Loonies, Toonies, Fives, Tens, Twenties, ")
        + QString("Fifties, Hundreds ")
        + QString("FROM CashFloat ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo));
    qDebug() << queryString;
    return queryResults->exec(queryString);
}

void DatabaseManager::getCashFloatThread(QDate date, int shiftNo)
{
    QString connName = QString::number(dbManager->getDbCounter());
    int nickels = 0, dimes = 0, quarters = 0, loonies = 0, toonies = 0,
        fives = 0, tens = 0, twenties = 0, fifties = 0, hundreds = 0;
    double total = 0;
    QString lastEditedEmpName("N/A");
    QString lastEditedDateTime("N/A");
    QString comments = "";
    QStringList list;
    {
        QSqlDatabase tempDb = QSqlDatabase::database();

        if (dbManager->createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);
            if (DatabaseManager::getCashFloatReportQuery(&query, date, shiftNo))
            {
                if (query.next())
                {
                    lastEditedEmpName = query.value(0).toString();
                    lastEditedDateTime = query.value(1).toString() + " at ";
                    lastEditedDateTime += query.value(2).toString();
                    comments = query.value(3).toString();
                    nickels = query.value(4).toInt();
                    dimes = query.value(5).toInt();
                    quarters = query.value(6).toInt();
                    loonies = query.value(7).toInt();
                    toonies = query.value(8).toInt();
                    fives = query.value(9).toInt();
                    tens = query.value(10).toInt();
                    twenties = query.value(11).toInt();
                    fifties = query.value(12).toInt();
                    hundreds = query.value(13).toInt();
                    total = nickels * VAL_NICKEL +
                            dimes * VAL_DIME +
                            quarters * VAL_QUARTER +
                            loonies * VAL_LOONIE +
                            toonies * VAL_TOONIE +
                            fives * VAL_FIVE +
                            tens * VAL_TEN +
                            twenties * VAL_TWENTY +
                            fifties * VAL_FIFTY +
                            hundreds * VAL_HUNDRED;
                }
            }
            tempDb.close();
        }   
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
  
    list << lastEditedEmpName << lastEditedDateTime << comments
         << QString::number(nickels) << QString::number(dimes)
         << QString::number(quarters) << QString::number(loonies)
         << QString::number(toonies) << QString::number(fives)
         << QString::number(tens) << QString::number(twenties)
         << QString::number(fifties) << QString::number(hundreds)
         << QString("$ " + QString::number(total, 'f', 2));

    emit DatabaseManager::cashFloatChanged(date, shiftNo, list);
}

bool DatabaseManager::insertCashFloat(QDate date, int shiftNo, QString empName,
    QString comments, QList<int> coins)
{
    bool ret = false;
    QString currentDateStr = QDate::currentDate().toString(Qt::ISODate);
    QString currentTimeStr = QTime::currentTime().toString("hh:mm:ss");
    QString connName = QString::number(DatabaseManager::getDbCounter());
    {
        QSqlDatabase tempDb = QSqlDatabase::database();

        if (DatabaseManager::createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);

            query.prepare(QString("INSERT INTO CashFloat ")
                + QString("(Date, ShiftNo, EmpName, DateEdited, TimeEdited, ")
                + QString("Comments, Nickels, Dimes, Quarters, Loonies, Toonies, ")
                + QString("Fives, Tens, Twenties, Fifties, Hundreds) ")
                + QString("VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

            query.addBindValue(date.toString(Qt::ISODate));
            query.addBindValue(shiftNo);
            query.addBindValue(empName);
            query.addBindValue(currentDateStr);
            query.addBindValue(currentTimeStr);
            query.addBindValue(comments);

            for (int i = 0; i < coins.size(); ++i)
            {
                query.addBindValue(coins.at(i));
            }

            if (query.exec())
            {
                qDebug() << "new cashfloat entry";
                ret = true;
            }
            else
            {
                query.prepare(QString("UPDATE CashFloat ")
                    + QString("SET EmpName = ? , DateEdited = ?, TimeEdited = ?, ")
                    + QString("Comments = ?, ")
                    + QString("Nickels = ?, Dimes = ?, Quarters = ?, ")
                    + QString("Loonies = ?, Toonies = ?, Fives = ?, Tens = ?, ")
                    + QString("Twenties = ?, Fifties = ?, Hundreds = ? ")
                    + QString("WHERE Date = '" + date.toString(Qt::ISODate))
                    + QString("' AND ShiftNo = " + QString::number(shiftNo)));

                query.addBindValue(empName);
                query.addBindValue(currentDateStr);
                query.addBindValue(currentTimeStr);
                query.addBindValue(comments);
                
                for (int i = 0; i < coins.size(); ++i)
                {
                    query.addBindValue(coins.at(i));
                }

                if (query.exec())
                {
                    qDebug() << "existing cashfloat updated";
                    ret = true;
                }
                else
                {
                    qDebug() << "inserting and updating cashfloat failed";
                } 
            }
        }
        tempDb.close();
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
    if (ret)
    {
        emit DatabaseManager::cashFloatInserted(empName, currentDateStr, currentTimeStr);
    }
    return ret;
}

int DatabaseManager::getIntFromQuery(QString queryString)
{
    int result = -1;
    QString connName = QString::number(DatabaseManager::getDbCounter());
    {
        QSqlDatabase tempDb = QSqlDatabase::database();
        if (DatabaseManager::createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);

            qDebug() << queryString;
            if (query.exec(queryString))
            {
                query.next();
                result = query.value(0).toInt();
            }
        }
        tempDb.close();
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
    return result;
}



/*==============================================================================
REPORT QUERYS (END)
==============================================================================*/

QSqlQuery DatabaseManager::getPrograms(){
    QSqlQuery query(db);
    QString q = "SELECT ProgramCode from Program";
    query.exec(q);
    return query;
}
bool DatabaseManager::removeTransaction(QString id){
    QSqlQuery query(db);
    QString curDate = QDate::currentDate().toString(Qt::ISODate);
    QString q = "UPDATE Transac Set Deleted = 1, Date = '" + curDate + "' WHERE TransacId ='" + id + "'";
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;

}
bool DatabaseManager::setPaid(QString id){
    QSqlQuery query(db);
    QString curDate = QDate::currentDate().toString(Qt::ISODate);
    QString q = "UPDATE Transac SET Outstanding = 0, Date = '" + curDate + "' WHERE TransacId ='" + id + "'";
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;
}
QSqlQuery DatabaseManager::getOutstanding(){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Transac JOIN Client on Transac.ClientId = Client.ClientId WHERE Outstanding = 1";
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
double DatabaseManager::getRoomCost(QString roomNo){
    QSqlQuery query(db);
    QString q = "SELECT cost from Space WHERE SpaceId='" + roomNo + "'";

    if(query.exec(q)){
        query.seek(0, false);
        return query.record().value(0).toString().toDouble();
    }
    return -1;
}

int DatabaseManager::getMonthlyRate(QString room, QString program){
    QSqlQuery query(db);
    QString q = "SELECT Monthly FROM Space WHERE SpaceId = '" + room + "'";
    if(query.exec(q)){
        return query.value(0).toInt();
    }
    else{
        return -1;
    }
}
QSqlQuery DatabaseManager::pullClient(QString id){
    QSqlQuery query(db);
    QString q = "SELECT * FROM CLIENT WHERE ClientId ='" + id + "'";
    if(!query.exec(q)){
        qDebug() << "ERROR IN PULLCLIENT";
    }
    return query;
}
bool DatabaseManager::updateBalance(double d, QString id){
    QSqlQuery query(db);
    QString q = "UPDATE Client SET Balance = '" + QString::number(d, 'f', 2)
            + "' WHERE ClientId ='" + id + "'";
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;
}
QSqlQuery DatabaseManager::getNextBooking(QDate endDate, QString roomId){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Booking WHERE StartDate >= '" + endDate.toString(Qt::ISODate) + "' AND SpaceId = '" + roomId + "'";
    query.exec(q);
    qDebug() << q;
    return query;
}

QSqlQuery DatabaseManager::getCurrentBooking(QDate start, QDate end, QString program){
    QSqlQuery query(db);
    QString q = "SELECT Space.SpaceId, Space.SpaceCode, Space.ProgramCodes, Space.type, Space.cost, Space.Monthly FROM Space"
                " LEFT JOIN (SELECT * from Booking WHERE StartDate <= '" + start.toString(Qt::ISODate) + "' AND EndDate > '"
                + start.toString(Qt::ISODate) + "') AS a on Space.SpaceId = a.SpaceId WHERE BookingID IS NULL AND Space.ProgramCodes = '" + program + "'";

    qDebug() << q;
    query.exec(q);
    return query;
}
bool DatabaseManager::updateBooking(QString q){
    QSqlQuery query(db);
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;
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

QSqlQuery DatabaseManager::updateEmployee(QString username, QString password, QString role) {
    QSqlQuery query(db);

    query.exec("UPDATE Employee SET Password='" + password + "', Role='" + role + "'WHERE Username='" + username + "';");

    return query;
}

QSqlQuery DatabaseManager::deleteEmployee(QString username, QString password, QString role) {
    QSqlQuery query(db);

    query.exec("DELETE FROM Employee WHERE Username='" + username
               + "' AND Password='" + password + "' AND Role='" + role + "';");

    return query;
}


QSqlQuery DatabaseManager::getActiveBooking(QString user, bool userLook){
    QSqlQuery query(db);
    QString date = QDate::currentDate().toString(Qt::ISODate);
    QString q;
    if(!userLook){
         q = "SELECT * FROM BOOKING JOIN Space on Booking.SpaceId = Space.SpaceId WHERE FirstBook = 'YES' AND EndDate >= '" + date + "'";
    }
    else{
         q = "SELECT * FROM BOOKING JOIN Space on Booking.SpaceId = Space.SpaceId WHERE FirstBook = 'YES' AND EndDate >= '" + date + "' AND ClientName LIKE '%" + user + "%'";

    }
    qDebug() << q;
    if(query.exec(q)){

    }
    else{
        qDebug() << "ERROR TRYING TO GET BOOKING";
    }
    return query;
}

QSqlQuery DatabaseManager::AddProgram(QString pcode, QString pdesc) {
    QSqlQuery query(db);

    query.exec("INSERT INTO Program VALUES('" + pcode +"', '" + pdesc + "')");

    return query;
}

QSqlQuery DatabaseManager::getAvailableBeds(QString pcode) {
    QSqlQuery query(db);

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly, s.ProgramCodes "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE s.ProgramCodes NOT LIKE '%" + pcode + "%'");

    // qDebug() << ":" + pcode+ ":";

    return query;
}

QSqlQuery DatabaseManager::getAssignedBeds(QString pcode) {
    QSqlQuery query(db);

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly, s.ProgramCodes "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE s.ProgramCodes LIKE '%" + pcode + "%'");

    return query;
}

QSqlQuery DatabaseManager::searchSingleBed(QString buildingno, QString floorno, QString roomno, QString spaceno) {
    QSqlQuery query(db);

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly, s.ProgramCodes "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE b.BuildingNo =" + buildingno + " "
               "AND f.FloorNo =" + floorno + " "
               "AND r.RoomNo =" + roomno + " "
               "AND s.SpaceNo =" + spaceno);

    qDebug() << "SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly, s.ProgramCodes "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE b.BuildingNo =" + buildingno + " "
               "AND f.FloorNo =" + floorno + " "
               "AND r.RoomNo =" + roomno + " "
               "AND s.SpaceNo =" + spaceno;

    return query;
}

QSqlQuery DatabaseManager::searchIDInformation(QString buildingno, QString floorno, QString roomno) {
    QSqlQuery query(db);

    query.exec("SELECT r.RoomId "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE b.BuildingNo =" + buildingno + " "
               "AND f.FloorNo =" + floorno + " "
               "AND r.RoomNo =" + roomno + " ");

    return query;
}

QSqlQuery DatabaseManager::deleteSpace(QString buildingno, QString floorno, QString roomno, QString spaceno) {
    QSqlQuery query(db);

    query.exec("DELETE FROM s "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE b.BuildingNo =" + buildingno + " "
               "AND f.FloorNo =" + floorno + " "
               "AND r.RoomNo =" + roomno + " "
               "AND s.SpaceNo =" + spaceno + " ");

    return query;
}

QSqlQuery DatabaseManager::updateAllSpacecodes() {
    QSqlQuery query(db);

    query.exec("UPDATE s "
               "SET s.SpaceCode =  "
               "CAST(b.BuildingNo AS VARCHAR(8)) + '-' +  "
               "CAST(f.FloorNo AS VARCHAR(8)) + '-' +  "
               "CAST(r.RoomNo AS VARCHAR(8)) + '-' +  "
               "CAST(s.SpaceNo AS VARCHAR(8)) +  "
               "LEFT(s.type, 1) "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId "
                   "INNER JOIN Floor f ON r.FloorId = f.FloorId "
                   "INNER JOIN Building b ON f.BuildingId = b.BuildingId ");

    return query;
}


QSqlQuery DatabaseManager::updateProgram(QString pcode, QString pdesc) {
    QSqlQuery query(db);

    query.exec("UPDATE Program SET Description='" + pdesc + "' WHERE ProgramCode='" + pcode + "'");

    return query;
}

QSqlQuery DatabaseManager::updateSpaceProgram(QString spaceid, QString program) {
    QSqlQuery query(db);

    query.exec("UPDATE Space SET ProgramCodes='" + program + "' WHERE SpaceId=" + spaceid);

    return query;
}

QSqlQuery DatabaseManager::addPcp(int rowId, QString clientId, QString type, QString goal, QString strategy, QString date) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO Pcp (rowId, clientId, Type, Goal, Strategy, Date) "
                  "VALUES (?, ?, ?, ?, ? ,?)");
    query.addBindValue(QString::number(rowId));
    query.addBindValue(clientId);
    query.addBindValue(type);
    query.addBindValue(goal);
    query.addBindValue(strategy);
    query.addBindValue(date);
    query.exec();

    return query;
}

QSqlQuery DatabaseManager::deletePcpRow(int rowId, QString type) {
    QSqlQuery query(db);

    query.exec("DELETE FROM Pcp WHERE Type = '" + type + "' AND rowId = " + QString::number(rowId));

    return query;
}

QSqlQuery DatabaseManager::addNote(QString clientId, QString notes) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO RunningNotes (ClientId, Notes) "
                  "VALUES (?, ?)");

    query.addBindValue(clientId);
    query.addBindValue(notes);

    query.exec();

    return query;
}

QSqlQuery DatabaseManager::updateNote(QString clientId, QString notes) {
    QSqlQuery query(db);

    query.prepare("UPDATE RunningNotes SET Notes=:notes WHERE ClientId=:client");

    query.bindValue(":notes", notes);
    query.bindValue(":client", clientId);

    query.exec();

    return query;
}

QSqlQuery DatabaseManager::readNote(QString clientId) {
    QSqlQuery query(db);

    query.exec("SELECT Notes FROM RunningNotes WHERE ClientId = " + clientId);

    return query;
}
