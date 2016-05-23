#include "databasemanager.h"
#include "shared.h"


int DatabaseManager::dbCounter = 0;
QMutex DatabaseManager::mutex;
QString DatabaseManager::staticError = "";

//ini file stuff
QSettings settings;
QString SERVER_NAME;
QString DB_NAME;
QString DB_USERNAME;
QString DB_PW;
QString DB_DSN;

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
   QSettings settings(QSettings::IniFormat, QSettings::SystemScope,
                      "The Salvation Army", "ARCWay");

   qDebug() << "ini path" << settings.fileName();
   settings.beginGroup("database");
   qDebug() << "server name: " << settings.value("SERVER_NAME").toString();
   //hack to make this work with this particular server name that has backslashes and commas
   QVariant value = settings.value("SERVER_NAME2");
   QString server_name2;
   if (value.type() == QVariant::StringList) {
     server_name2 = value.toStringList().join(",");
   } else {
     server_name2 = value.toString();
   }
   qDebug() << "server name part 2: " << server_name2;
   qDebug() << "db name: " << settings.value("DB_NAME").toString();
   qDebug() << "db username: " << settings.value("DB_USERNAME").toString();
   qDebug() << "db pw: " << settings.value("DB_PW").toString();

//   //set constants

   SERVER_NAME = settings.value("SERVER_NAME").toString() + "\\" + server_name2;
   DB_NAME = settings.value("DB_NAME").toString();
   DB_USERNAME = settings.value("DB_USERNAME").toString();
   DB_PW = settings.value("DB_PW").toString();
   DB_DSN = settings.value("DB_DSN").toString();

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

    QString dsn = DB_DSN;
//    QString dsn =
//            QString("DRIVER={SQL Server};SERVER=%1;DATABASE=%2;UID=%3;PWD=%4;").
//            arg(SERVER_NAME).arg(DB_NAME).arg(DB_USERNAME).arg(DB_PW);

    tempDbPtr->setDatabaseName(dsn);


    if (!tempDbPtr->open())
    {
        QString error = tempDbPtr->lastError().text().toLocal8Bit().data();
        qDebug() << error;
        staticError = error;
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
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    query.exec("SELECT * FROM " + tableName);
    return query;
}

void DatabaseManager::reconnectToDatabase()
{
    if (!db.open())
    {
        emit noDatabaseConnection(&db);
    }
    else
    {
        qDebug() << "reconnection successful";
        emit reconnectedToDatabase();
    }
}

void DatabaseManager::reconnectToDatabase(QSqlDatabase* database)
{
    if (!database->open())
    {
        emit noDatabaseConnection(database);
    }
    else
    {
        qDebug() << "reconnection successful";
        emit reconnectedToDatabase();
    }
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
bool DatabaseManager::deleteBooking(QString id){
    QSqlQuery query(db);
    QString q = "DELETE FROM Booking WHERE BookingId ='" + id + "'";
    qDebug() << q;
    return query.exec(q);
}
QSqlQuery DatabaseManager::getRole(QString empName){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Employee WHERE Username ='" + empName + "'";
    query.exec(q);
    return query;
}
double DatabaseManager::getBookingCost(QString bookId){
    QSqlQuery query(db);
    QString q = "SELECT Cost from Booking WHERE BookingId = ' "+ bookId + "'";
    query.exec(q);
    if(!query.next())
        return 0;
    double realCost = query.value("Cost").toString().toDouble();
    return realCost;
}

QSqlQuery DatabaseManager::execQuery(QString queryString)
{
    qDebug()<<"execQuery. ";
    if (!db.open())
    {
        emit noDatabaseConnection(&db);
    }

    QSqlQuery query(db);
    query.exec(queryString);
    return query;
}
QSqlQuery DatabaseManager::loadStorage(QString id){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Storage WHERE ClientId ='" + id + "' AND Deleted = 0";
    query.exec(q);
    return query;
}
bool DatabaseManager::addStorage(QString id, QString name, QString data){
    QSqlQuery query(db);
    QString q = "INSERT INTO Storage VALUES ('" + QDate::currentDate().toString(Qt::ISODate) + "',' " + id + "','" + name +"', '', '0' )";
    return query.exec(q);

}
bool DatabaseManager::removeStorage(QString storeId){
    QSqlQuery query(db);
    QString q = "UPDATE Storage SET Deleted = '1' WHERE StorageId = '" + storeId + "'";
    return query.exec(q);
}

bool DatabaseManager::updateStoreDate(QString storeId){
    QSqlQuery query(db);
    QString q = "UPDATE Storage SET StorageDate ='" + QDate::currentDate().toString(Qt::ISODate) + "'";
    return query.exec(q);
}
QSqlQuery DatabaseManager::getFullStorage(){
    QSqlQuery query(db);
    QString q = "SELECT * FROM Storage WHERE Deleted = 0";
    query.exec(q);
    return query;

}

bool DatabaseManager::updateStore(QString storeId, QString data){
    QSqlQuery query(db);
    query.prepare("UPDATE Storage SET StorageItems = :store WHERE StorageId = :storeId");
    query.bindValue(":store", data);
    query.bindValue(":storeId", storeId);
    return query.exec();
}

bool DatabaseManager::execQuery(QSqlQuery* query, QString queryString)
{
    return query->exec(queryString);
}

bool DatabaseManager::checkDatabaseConnection()
{
    if(!db.open())
    {
        emit noDatabaseConnection();
        return false;
    }
    return true;
}

void DatabaseManager::checkDatabaseConnection(QSqlDatabase* database)
{
    if (!database->open())
    {
        emit noDatabaseConnection(database);
    }
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
       // qDebug() << "[[" << current << "]]";//UNCOMMENT
    }
}
/*==============================================================================
FILE DOWLOAD AND UPLOAD RELATED FUNCTIONS (END)
==============================================================================*/


/*==============================================================================
SEARCH CLIENT LIST FUNCTION(START)
==============================================================================*/
//get client list
QSqlQuery DatabaseManager::searchClientList(QString ClientName){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    //default select query
    QString searchQuery = QString("SELECT ClientId, LastName, FirstName, MiddleName, Dob, Balance ")
            + QString("FROM Client ");
    QStringList clientNames;
    if(ClientName.toLower() == "anonymous"){
            qDebug()<<"case: " + ClientName.toLower();
            searchQuery += QString("WHERE FirstName Like '"+ ClientName.toLower() + "' ")
                         + QString("OR LastName Like '"+ ClientName.toLower() + "' ");

    }
    else if(ClientName != ""){
        clientNames = ClientName.split(" ");
    }

    if(clientNames.count() == 1){
        qDebug()<<"Name 1 words";
        searchQuery += QString("WHERE (FirstName LIKE '"+clientNames.at(0) + "%' ")
                     + QString("OR LastName Like '"+clientNames.at(0)+"%') ")
                     + QString("AND NOT FirstName = 'anonymous' ");
    }
    //if 2 word name, match first name and last name
    else if(clientNames.count() == 2){
        qDebug() << "Name 2 words";
        searchQuery += QString("WHERE (LastName LIKE '"+clientNames.at(0) + "%' AND FirstName LIKE '" + clientNames.at(1) + "%') ")
                     + QString("OR (FirstName Like '"+clientNames.at(0)+"%' AND LastName LIKE '" + clientNames.at(1) + "%') ")
                     + QString("AND NOT FirstName = 'anonymous' ");
    }
    else{
        if(ClientName ==""){
            qDebug()<<"DB no name";
            searchQuery+= QString("WHERE NOT ISNULL(FirstName, '') = 'anonymous' ");
        }else
            qDebug()<<"Wrong name";
    }
    searchQuery += QString("ORDER BY LastName ASC, FirstName ASC");
    query.prepare(searchQuery);
    query.exec();
    return query;

}

//search all information in the a table for 1 client
QSqlQuery DatabaseManager::searchTableClientInfo(QString tableName, QString ClientId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery selectquery(db);
    selectquery.prepare(QString("SELECT * FROM ")+ tableName
                      + QString(" WHERE ClientId = ") + ClientId);
    selectquery.exec();
    return selectquery;

}

//get client information (without picture
QSqlQuery DatabaseManager::searchClientInfo(QString ClientId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery selectquery(db);

    selectquery.prepare(QString("SELECT FirstName, MiddleName, LastName, Dob, Balance, ")
                      + QString("SinNo, GaNo, EmpId, DateRulesSigned, status, ")
                      + QString("NokName, NokRelationship, NokLocation, NokContactNo, PhysName, ")
                      + QString("PhysContactNo, SuppWorker1Name, SuppWorker1ContactNo, SuppWorker2Name, SuppWorker2ContactNo, ")
                      + QString("Comments ")
                      + QString("FROM Client WHERE ClientId =" + ClientId));


    selectquery.exec();

  //  printAll(selectquery);
    return selectquery;
}

//get client profile picture(NEW CONNECTION USE)
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

//SEARCH CLIENT TRANSACTION LIST (CLIENT INFO , CASEFILE TRANSACTION LIST)
QSqlQuery DatabaseManager::searchClientTransList(int maxNum, QString clientId, int type = 0){
    QSqlQuery clientTransQuery;
    QString queryStart;
    if(type == 1){   //CLIENT INFO PAGE
        queryStart = "SELECT TOP "+ QString::number(maxNum)
                   + QString(" Date, TransType, Amount, Type, EmpName , ChequeNo, MSQ, ChequeDate ");
    }else{          //CASEFILE PAGE
        queryStart = "SELECT "
                   + QString(" CAST(Date AS datetime) + CAST(Time AS datetime) AS Datetime, TransType, Amount, Type, ChequeNo, MSQ, ChequeDate, Deleted, Outstanding, EmpName, Notes ");
    }
    queryStart += QString("FROM Transac ")
            + QString("WHERE ClientId = " + clientId + " ORDER BY Date DESC, Time DESC");
    clientTransQuery.prepare(queryStart);
    qDebug()<<"QERY: "<< queryStart;
    clientTransQuery.exec();
    return clientTransQuery;
}


QSqlQuery DatabaseManager::searchBookList(int maxNum, QString clientId, int type){
    QSqlQuery clientBookingQuery;
    QString queryStart;
    if(type == 1){  //CLIENT INFORMATION PAGE
        queryStart = "SELECT TOP "+ QString::number(maxNum)
                   + QString("b.DateCreated, b.programCode, b.StartDate, b.EndDate, b.Cost, s.SpaceCode ");
    }
    else{          //CASEFILE PAGE
        queryStart = "SELECT "
                   + QString("b.DateCreated, b.ProgramCode, b.StartDate, b.EndDate, b.Cost, s.SpaceCode, b.FirstBook, b.Monthly ");
    }
    clientBookingQuery.prepare(queryStart
                           + QString("FROM Booking b INNER JOIN Space s ON b.SpaceId = s.SpaceId ")
                           + QString("WHERE ClientId = " + clientId + " ORDER BY EndDate DESC, DateCreated DESC"));
    clientBookingQuery.exec();
    return clientBookingQuery;
}

/*-------------------------------------------------------
 * COUNT HOW MANY INFO A CLIENT HAVE IN THE TABLE
 - tableName : the table name want to search
 - ClientId  : the client id to search
  -------------------------------------------------------*/
int DatabaseManager::countInformationPerClient(QString tableName, QString ClientId){
    QSqlQuery countQuery;
    countQuery.prepare(QString("SELECT COUNT(*) FROM " + tableName )
                     + QString(" WHERE ClientId = " + ClientId));
    countQuery.exec();
    countQuery.next();
//    qDebug()<<"QUERY Count " + countQuery.value(0).toString();
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



bool DatabaseManager::insertIntoBookingHistory(QString clientName, QString spaceId, QString program, QString start, QString end, QString action, QString emp, QString shift, QString clientId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "INSERT INTO BookingHistory VALUES ('" +clientName + "','" +
            spaceId + "','" + program + "','" + QDate::currentDate().toString(Qt::ISODate)
            + "','" + start + "','" + end + "','" + action + "','" + "UNKNOWN"
            + "','" + emp + "','" + shift + "','" + QTime::currentTime().toString()
            + "','" + clientId + "')";
    qDebug() << q;
    return query.exec(q);
}

bool DatabaseManager::addHistoryFromId(QString bookId, QString empId, QString shift, QString action){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Booking JOIN Space on Booking.SpaceId = Space.SpaceId WHERE BookingId = '" + bookId + "'";
    if(!query.exec(q)){
        return false;
    }
    while(query.next()){
        QString clientName =query.value("ClientName").toString();
        QString spaceId = query.value("SpaceCode").toString();
        QString program = query.value("ProgramCode").toString();
        QString start = query.value("StartDate").toString();
        QString end = query.value("EndDate").toString();
        QString clientId = query.value("ClientId").toString();
        return insertIntoBookingHistory(clientName, spaceId, program, start, end, action, empId, shift, clientId );

    }

}
QSqlQuery DatabaseManager::getBalance(QString clientId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT Balance FROM Client WHERE ClientId ='" + clientId + "'";
    query.exec(q);
    return query;
}

QSqlQuery DatabaseManager::getRoomCosts(QString roomId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT cost, Monthly FROM Space WHERE SpaceId ='" + roomId + "'";
    query.exec(q);
    return query;
}


/*================================================================
    CLIENT REGISTER
=================================================================*/
//NEW CLIENT
bool DatabaseManager::insertClientWithPic(QStringList* registerFieldList, QImage* profilePic)//QObject sth, QImage profilePic)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    profilePic->save(&buffer, "PNG");  // writes image into ba in PNG format
    DatabaseManager::checkDatabaseConnection(&db);
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
           // qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
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

//UPDATE CLIENT INFORMATION (EDIT CLIENT INFO)
bool DatabaseManager::updateClientWithPic(QStringList* registerFieldList, QString clientId, QImage* profilePic)//QObject sth, QImage profilePic)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    profilePic->save(&buffer, "PNG");  // writes image into ba in PNG format

    DatabaseManager::checkDatabaseConnection(&db);
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
           // qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
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


//INSERT CLIENT REGISTER OR UPDATE HISTORY TO LOG
bool DatabaseManager::insertClientLog(QStringList* registerFieldList)
{
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.prepare(QString("INSERT INTO ClientLog ")
        + QString("(ClientName, Action, Date, ShiftNo, Time, EmpName) ")
        + QString("VALUES(?, ?, ?, ?, ?, ?)"));

    for (int i = 0; i < registerFieldList->size(); ++i)
    {
        if (registerFieldList->at(i) != NULL)
        {
           // qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
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

bool DatabaseManager::deleteClientFromTable(QString tableName, QString ClientId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    query.prepare(QString("DELETE FROM "+tableName)
                  += QString(" WHERE ClientId = " + ClientId));
    return query.exec();
}

/* .............................................................
         CLIENT REGISTER FINISHED
  ==============================================================*/

QSqlQuery DatabaseManager::getBooking(QString bId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Booking where BookingId ='" + bId + "'";
    query.exec(q);
    return query;

}

bool DatabaseManager::updateWakeupRoom(QDate startDate, QDate endDate, QString clientId, QString rooomId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "UPDATE Wakeup SET SpaceCode ='" + rooomId +"' WHERE WakeDate >= '" + startDate.toString(Qt::ISODate) + "' AND WakeDate <= '" + endDate.toString(Qt::ISODate)
            + "' AND ClientId = '" + clientId +"'";
    qDebug() <<q;
    return query.exec(q);
}

QSqlQuery DatabaseManager::getTransactions(QDate start, QDate end){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Transac JOIN Client on Transac.ClientId = Client.ClientID WHERE Date >= '" + start.toString(Qt::ISODate) + "' AND Date <= '"
            + end.toString(Qt::ISODate) + "'  AND Deleted = 0";
    qDebug() << q;
    query.exec(q);
    return query;
}
QSqlQuery DatabaseManager::getOwingClients(){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Client WHERE Balance < 0";
    query.exec(q);
    return query;
}
QSqlQuery DatabaseManager::setLunches(QDate date, int num, QString id, QString room){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "INSERT INTO Lunches Values('" + id + "','" + date.toString(Qt::ISODate) + "','" + QString::number(num) + "', '" + room + "' )";
    qDebug() << q;
    query.exec(q);
    return query;

}
QSqlQuery DatabaseManager::getLunches(QDate start, QDate end, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Lunches WHERE ClientId = '" + id + "' AND LunchDate >= '" + start.toString(Qt::ISODate)
            + "' AND LunchDate <= '" + end.toString(Qt::ISODate) + "'";
    qDebug() << q;
    query.exec(q);
    return query;
 }
bool DatabaseManager::updateLunches(QDate date, int num, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "UPDATE Lunches SET Number ='" + QString::number(num) + "' WHERE LunchDate = '"
            + date.toString(Qt::ISODate) + "' AND ClientId = '" + id + "'";
    qDebug() << q;
    return query.exec(q);
}
QSqlQuery DatabaseManager::getWakeups(QDate start, QDate end, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Wakeup WHERE ClientId = '" + id + "' AND WakeDate >= '" + start.toString(Qt::ISODate)
            + "' AND WakeDate <= '" + end.toString(Qt::ISODate) + "'";
    qDebug() << q;
    query.exec(q);
    return query;
}
bool DatabaseManager::deleteWakeups(QDate date, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "DELETE FROM Wakeup WHERE ClientId ='" + id + "' AND WakeDate = '" + date.toString(Qt::ISODate) + "'";
    qDebug() << q;
    return query.exec(q);
}
bool DatabaseManager::deleteWakeupsMulti(QDate date, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "DELETE FROM Wakeup WHERE ClientId ='" + id + "' AND WakeDate >= '" + date.toString(Qt::ISODate) + "'";
    qDebug() << q;
    return query.exec(q);
}
bool DatabaseManager::updateWakeups(QDate date, QString time, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "UPDATE Wakeup SET WakeTime ='" + time + "' WHERE WakeDate = '"
            + date.toString(Qt::ISODate) + "' AND ClientId = '" + id + "'";
    qDebug() << q;
    return query.exec(q);
}

bool DatabaseManager::setWakeup(QDate date, QString time, QString id, QString room){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "INSERT INTO Wakeup Values('" + id + "','" + date.toString(Qt::ISODate) + "','" + time + "','" + room +"')";
    qDebug() << q;
    return query.exec(q);

}
bool DatabaseManager::removeLunchesMulti(QDate date, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "DELETE FROM Lunches WHERE LunchDate >= '" + date.toString(Qt::ISODate) + "' AND ClientID ='"+
            id + "'";
    qDebug() << q;
    return query.exec(q);
}
bool DatabaseManager::removeLunches(QDate date, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
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
        QString("SELECT b.ClientName, s.SpaceCode, b.StartDate, ")
        + QString("b.EndDate, b.ProgramCode, c.EspDays, REPLACE('$' + CAST(c.Balance AS VARCHAR), '$-', '-$') ")
        + QString("FROM Booking b INNER JOIN Client c ON b.ClientId = c.ClientId ")
        + QString("INNER JOIN Space s ON b.SpaceId = s.SpaceId ")
        + QString("WHERE EndDate = '" + date.toString(Qt::ISODate))
        + QString("' AND FirstBook = 'YES' ORDER BY b.ProgramCode Desc" );

        // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getDailyReportVacancyQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
        QString("SELECT s.SpaceCode, s.ProgramCodes ")
        + QString("FROM Space s LEFT JOIN (SELECT SpaceId, Date ")
        + QString("FROM Booking WHERE StartDate <= '" + date.toString(Qt::ISODate))
        + QString("' AND EndDate > '" + date.toString(Qt::ISODate))
        + QString("') as b ON s.SpaceId = b.SpaceId ")
        + QString("WHERE b.date IS NULL ORDER BY s.ProgramCodes Desc");

        // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getDailyReportLunchQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
        QString("SELECT ISNULL(c.LastName, '') + ', ' + ISNULL(c.FirstName, '') ")
        + QString("+ ' ' + ISNULL(c.MiddleName, ''), l.SpaceCode, l.Number ")
        + QString("FROM Lunches l INNER JOIN Client c ON l.ClientId = c.ClientId ")
        + QString("WHERE LunchDate = '" + date.toString(Qt::ISODate) + "'");

        //qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getDailyReportWakeupQuery(QSqlQuery* queryResults, QDate date)
{
    QString queryString =
            QString("SELECT ISNULL(c.LastName, '') + ', ' + ISNULL(c.FirstName, '') ")
            + QString("+ ' ' + ISNULL(c.MiddleName, ''), w.SpaceCode, ")
            + QString("CONVERT(VARCHAR(15), CAST(w.WakeTime AS TIME(0)) , 100) ")
            + QString("FROM Wakeup w INNER JOIN Client c ON w.ClientId = c.ClientId ")
            + QString("WHERE WakeDate = '" + date.toString(Qt::ISODate) + "' ")
            + QString("ORDER BY CAST(w.WakeTime AS TIME(0)) Asc");

    // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getShiftReportBookingQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString =
        QString("SELECT ClientName, SpaceCode, ProgramCode, StartDate, ")
        + QString("EndDate, Action, EmpName, CONVERT(VARCHAR(15), Time, 100) ")
        + QString("FROM BookingHistory ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo))
        + QString(" ORDER BY Time Asc");
    //qDebug() << queryString;
    return queryResults->exec(queryString);
}
QSqlQuery DatabaseManager::getSwapBookings(QDate start, QDate end, QString program, QString maxSwap, QString curBook){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM (SELECT COUNT(SpaceID) C, SpaceId FROM Booking"
                " WHERE EndDate > '" + start.toString(Qt::ISODate) + "' AND StartDate <= '" + end.toString(Qt::ISODate)
            +"' AND EndDate <= '" + maxSwap +"' GROUP BY SpaceId ) AS T LEFT JOIN (SELECT * FROM Space) AS S on S.SpaceId = T.SpaceId" +
            " JOIN ( SELECT * FROM Booking WHERE EndDate > '" + start.toString(Qt::ISODate) + "' AND StartDate <= '" + end.toString(Qt::ISODate)
            +"' AND EndDate <= '" + maxSwap +"') V on T.SpaceId = V.SpaceId WHERE C = 1 AND BookingId != '" + curBook + "' AND EndDate > '" + QDate::currentDate().toString(Qt::ISODate) + "'" ;
    //AND ProgramCodes ='" + program + "' AND BookingId != '" + curBook + "'";

    query.exec(q);
    qDebug() << q;
    return query;
}
double DatabaseManager::getDoubleBalance(QString clientId){
    QSqlQuery query(db);
    QString q = "SELECT Balance FROM Client WHERE ClientId='" + clientId + "'";
    query.exec(q);
    if(!query.next()){
        return -1;
    }
    return query.value("Balance").toString().toDouble();
}

bool DatabaseManager::getShiftReportTransactionQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString =
        QString("SELECT c.FirstName, c.MiddleName, c.LastName, t.TransType, ")
        + QString("t.Type, t.Amount, t.MSQ, t.ChequeNo, t.ChequeDate, ")
        + QString("CAST(t.Outstanding AS INT), CAST(t.Deleted AS INT), ")
        + QString("t.EmpName, CONVERT(VARCHAR(15), t.Time, 100), t.Notes " )
        + QString("FROM Client c INNER JOIN Transac t ON c.ClientId = t.ClientId ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo))
        + QString(" ORDER BY t.Time Asc");
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
            + QString("(SELECT SpaceId, Date FROM Booking WHERE StartDate <= '")
            + QString(date.toString(Qt::ISODate) + "' AND EndDate > '")
            + QString(date.toString(Qt::ISODate) + "') as b ")
            + QString("ON s.SpaceId = b.SpaceId ")
            + QString("WHERE b.Date IS NULL AND s.ProgramCodes LIKE '%ESP%'");
    // qDebug() << queryString;
    return DatabaseManager::getIntFromQuery(queryString);
}

int DatabaseManager::getDailyReportTotalVacancies(QDate date)
{
    QString queryString =
            QString("SELECT COUNT(s.SpaceId) ")
            + QString("FROM SPACE s LEFT JOIN ")
            + QString("(SELECT SpaceId, Date FROM Booking WHERE StartDate <= '")
            + QString(date.toString(Qt::ISODate) + "' AND EndDate > '")
            + QString(date.toString(Qt::ISODate) + "') as b ")
            + QString("ON s.SpaceId = b.SpaceId ")
            + QString("WHERE b.Date IS NULL");
    // qDebug() << queryString;
    int test = DatabaseManager::getIntFromQuery(queryString);
    // qDebug() << "totalVacancies" << QString::number(test);
    return DatabaseManager::getIntFromQuery(queryString);
}

void DatabaseManager::getDailyReportStatsThread(QDate date)
{
    bool conn = false;
    QList<int> list;
    int numEspCheckouts, numTotalCheckouts, numEspVacancies, numTotalVacancies;
    if ((numEspCheckouts = DatabaseManager::getDailyReportEspCheckouts(date)) != -1) 
    {
        if ((numTotalCheckouts = DatabaseManager::getDailyReportTotalCheckouts(date)) != -1)
        {
            if ((numEspVacancies = DatabaseManager::getDailyReportEspVacancies(date)) != -1)
            {
                if ((numTotalVacancies = DatabaseManager::getDailyReportTotalVacancies(date)) != -1)
                {
                    list << numEspCheckouts << numTotalCheckouts << numEspVacancies
                         << numTotalVacancies;
                    conn = true;
                }        
            }
        }
    }
    
    emit DatabaseManager::dailyReportStatsChanged(list, conn);
}

bool DatabaseManager::getShiftReportTotal(QDate date, int shiftNo, QString payType,
    double* result)
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
    return DatabaseManager::getDoubleFromQuery(queryString, result);
}

void DatabaseManager::getShiftReportStatsThread(QDate date, int shiftNo)
{
    double cashTotal, electronicTotal, chequeTotal;
    QStringList list;
    bool conn = false;
    if (DatabaseManager::getShiftReportTotal(date, shiftNo, PAY_CASH, &cashTotal))
    {
        if (DatabaseManager::getShiftReportTotal(date, shiftNo, PAY_ELECTRONIC, &electronicTotal))
        {
            if (DatabaseManager::getShiftReportTotal(date, shiftNo, PAY_CHEQUE, &chequeTotal))
            {
                list << QString("%1%2").arg(cashTotal >= 0 ? "$" : "-$").
                    arg(QString::number(fabs(cashTotal), 'f', 2));
                list << QString("%1%2").arg(electronicTotal >= 0 ? "$" : "-$").
                    arg(QString::number(fabs(electronicTotal), 'f', 2));
                list << QString("%1%2").arg(chequeTotal >= 0 ? "$" : "-$").
                    arg(QString::number(fabs(chequeTotal), 'f', 2));
                list << QString("%1%2").arg(cashTotal + chequeTotal >= 0 ? "$" : "-$").
                    arg(QString::number(fabs(cashTotal + chequeTotal), 'f', 2));
                list << QString("%1%2").arg(cashTotal + electronicTotal + chequeTotal >= 0 ? "$" : "-$").
                    arg(QString::number(fabs(cashTotal + electronicTotal + chequeTotal), 'f', 2));
                conn = true;
            }        
        }    
    }
    emit DatabaseManager::shiftReportStatsChanged(list, conn);
}

bool DatabaseManager::getShiftReportClientLogQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString =
        QString("SELECT ClientName, Action, EmpName, CONVERT(VARCHAR(15), Time, 100) ")
        + QString("FROM ClientLog ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo))
        + QString("ORDER BY Time Desc");
    // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getShiftReportOtherQuery(QSqlQuery* queryResults,
    QDate date, int shiftNo)
{
    QString queryString = 
        QString("SELECT CONVERT(VARCHAR(15), Time, 100), EmpName, Log ")
        + QString("FROM OtherLog ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo) + " ")
        + QString("ORDER BY Time Desc");
    // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::insertOtherLog(QString empName, int shiftNo, QString logText)
{
    DatabaseManager::checkDatabaseConnection(&db);
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
        QString("SELECT EmpName, DateEdited, CONVERT(VARCHAR(15), TimeEdited, 100), ")
        + QString("Comments, Nickels, Dimes, Quarters, Loonies, Toonies, ")
        + QString("Fives, Tens, Twenties, Fifties, Hundreds ")
        + QString("FROM CashFloat ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate) + "' AND ")
        + QString("ShiftNo = " + QString::number(shiftNo));
    // qDebug() << queryString;
    return queryResults->exec(queryString);
}

void DatabaseManager::getCashFloatThread(QDate date, int shiftNo)
{
    QString connName = QString::number(dbManager->getDbCounter());
    int nickels = 0, dimes = 0, quarters = 0, loonies = 0, toonies = 0,
        fives = 0, tens = 0, twenties = 0, fifties = 0, hundreds = 0;
    double total = 0;
    bool conn = true;
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
            list << lastEditedEmpName << lastEditedDateTime << comments
                 << QString::number(nickels) << QString::number(dimes)
                 << QString::number(quarters) << QString::number(loonies)
                 << QString::number(toonies) << QString::number(fives)
                 << QString::number(tens) << QString::number(twenties)
                 << QString::number(fifties) << QString::number(hundreds)
                 << QString("$" + QString::number(total, 'f', 2));
        }
        else
        {
            tempDb.close();
            conn = false;
        }   
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);

    emit DatabaseManager::cashFloatChanged(date, shiftNo, list, conn);
}

bool DatabaseManager::insertCashFloat(QDate date, int shiftNo, QString empName,
    QString comments, QList<int> coins)
{
    bool ret = false;
    QTime currentTime(QTime::currentTime());
    QString currentDateStr = QDate::currentDate().toString(Qt::ISODate);
    QString currentTimeStr = currentTime.toString("hh:mm:ss");
    QString currentTimeStr12hr = currentTime.toString("h:mmAP");
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
            tempDb.close();
        }
        else
        {
            tempDb.close();
            return false;
        }
        
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
    if (ret)
    {
        emit DatabaseManager::cashFloatInserted(empName, currentDateStr, currentTimeStr12hr);
    }
    return ret;
}

bool DatabaseManager::getMonthlyReportQuery(QSqlQuery* queryResults, int month, int year)
{
    QString queryString = 
        QString("SELECT SUM(NumBedsUsed), SUM(NumVacancies), SUM(numSpaces), ")
        + QString("SUM(NumNewClients) ")
        + QString("FROM DailyStats ")
        + QString("WHERE MONTH(Date) = " + QString::number(month))
        + QString(" AND YEAR(Date) = " + QString::number(year));
    // qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getYellowRestrictionQuery(QSqlQuery* queryResults)
{
    QString queryString =
        QString("SELECT CASE WHEN ISNULL(LastName, '') = '' THEN '' ELSE LastName + ', ' END ")
        + QString("+ ISNULL(FirstName, '') + ' ' + ISNULL(MiddleName, '')" )
        + QString("FROM Client Where Status = 'Yellow'");
    return queryResults->exec(queryString);
}

bool DatabaseManager::getRedRestrictionQuery(QSqlQuery* queryResults)
{
    QString queryString =
        QString("SELECT CASE WHEN ISNULL(LastName, '') = '' THEN '' ELSE LastName + ', ' END ")
        + QString("+ ISNULL(FirstName, '') + ' ' + ISNULL(MiddleName, '')" )
        + QString("FROM Client Where Status = 'Red'");
    return queryResults->exec(queryString);
}

int DatabaseManager::getMonthlyUniqueClients(int month, int year)
{
    QDate firstDay(year, month, 1);
    QDate lastDay(firstDay.addDays(firstDay.daysInMonth() - 1));

    QString queryString =
            QString("SELECT COUNT(DISTINCT ClientId) ")
            + QString("FROM BookingHistory ")
            + QString("WHERE ((StartDate <= '" + firstDay.toString(Qt::ISODate) + "' AND ")
            + QString("EndDate > '" + firstDay.toString(Qt::ISODate) + "') OR ")
            + QString("(StartDate >= '" + firstDay.toString(Qt::ISODate) + "' AND ")
            + QString("StartDate <= '" + lastDay.toString(Qt::ISODate) + "')) ")
            + QString("AND ClientId <> 1 ") // Anonymous id = 1
            + QString("AND Action <> 'DELETE'");
    int numUniqueNamed = DatabaseManager::getIntFromQuery(queryString);

    queryString =
        QString("SELECT COUNT(ClientId) ")
        + QString("FROM BookingHistory ")
        + QString("WHERE ((StartDate <= '" + firstDay.toString(Qt::ISODate) + "' AND ")
        + QString("EndDate > '" + firstDay.toString(Qt::ISODate) + "') OR ")
        + QString("(StartDate >= '" + firstDay.toString(Qt::ISODate) + "' AND ")
        + QString("StartDate <= '" + lastDay.toString(Qt::ISODate) + "')) ")
        + QString("AND ClientId = 1 ") // Anonymous id = 1
        + QString("AND Action <> 'DELETE'");
    int numAnonymous = DatabaseManager::getIntFromQuery(queryString);
    // qDebug() << queryString;
    // qDebug() << "numAnonymous " + QString::number(numAnonymous);
    return numUniqueNamed + numAnonymous;
}

void DatabaseManager::getMonthlyReportThread(int month, int year)
{
    QString connName = QString::number(dbManager->getDbCounter());
    QStringList list;
    bool conn = true;
    {
        QSqlDatabase tempDb = QSqlDatabase::database();

        if (dbManager->createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);
            if (DatabaseManager::getMonthlyReportQuery(&query, month, year))
            {
                if (query.next())
                {
                    list << query.value(0).toString()
                         << query.value(1).toString()
                         << query.value(2).toString()
                         << query.value(3).toString()
                         << QString::number(DatabaseManager::getMonthlyUniqueClients(month, year));
                }
            }
            tempDb.close();
        }
        else
        {
            tempDb.close();
            conn = false;
        } 
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);

    emit DatabaseManager::monthlyReportChanged(list, conn);
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
                if (query.next())
                {
                    result = query.value(0).toInt();    
                }
            }
        }
        tempDb.close();
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
    return result;
}

bool DatabaseManager::getDoubleFromQuery(QString queryString, double* result)
{
    bool ret = false;
    QString connName = QString::number(DatabaseManager::getDbCounter());
    {
        QSqlDatabase tempDb = QSqlDatabase::database();
        if (DatabaseManager::createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);

            qDebug() << queryString;
            if (query.exec(queryString))
            {
                if (query.next())
                {
                    *result = query.value(0).toDouble();
                    ret = true;
                }
            }
        }
        tempDb.close();
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);
    return ret;
}



/*==============================================================================
REPORT QUERYS (END)
==============================================================================*/

QSqlQuery DatabaseManager::getPrograms(){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT ProgramCode from Program";
    query.exec(q);
    return query;
}
bool DatabaseManager::removeTransaction(QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString curDate = QDate::currentDate().toString(Qt::ISODate);
    QString q = "UPDATE Transac Set Deleted = 1, Date = '" + curDate + "' WHERE TransacId ='" + id + "'";
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;

}
bool DatabaseManager::setPaid(QString id, QString chequeNo){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString curDate = QDate::currentDate().toString(Qt::ISODate);
    QString q;
    if(chequeNo.length() > 0){
        q = "UPDATE Transac SET Outstanding = 0, Date = '" + curDate + "', ChequeNo ='" + chequeNo + "' WHERE TransacId ='" + id + "'";

    }else{

        q = "UPDATE Transac SET Outstanding = 0, Date = '" + curDate + "' WHERE TransacId ='" + id + "'";
    }
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;
}
bool DatabaseManager::escapePayment(QString clientId, QString curDate, QString amount, QString type, QString notes, QString chequeNo, QString msd, QString issued,
                                    QString transtype, QString outstanding, QString empId, QString shiftNo, QString time){

    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    query.prepare("INSERT INTO Transac VALUES(:clientId, :curDate, :amount, :type, :notes, :chequeNo, :msd, :issued, :transtype, :deleted, :outstanding, :empId, :shiftNo, :time)");

    query.bindValue(":clientId", clientId);
    query.bindValue(":curdate", QDate::currentDate());
    query.bindValue(":amount",amount );
    query.bindValue(":type", type );
    query.bindValue(":notes",notes );
    query.bindValue(":chequeNo", chequeNo);
    query.bindValue(":msd",msd );
    query.bindValue(":issued",issued );
    query.bindValue(":transtype",transtype );
    query.bindValue(":deleted", "0");
    query.bindValue(":outstanding",outstanding );
    query.bindValue(":empId",empId );
    query.bindValue(":shiftNo",shiftNo );
    query.bindValue(":time",QTime::currentTime() );
    return query.exec();


}

QSqlQuery DatabaseManager::getOutstanding(){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Transac JOIN Client on Transac.ClientId = Client.ClientId WHERE Outstanding = 1";
    query.exec(q);
    return query;
}

bool DatabaseManager::addPayment(QString values){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "INSERT INTO Transac Values( " + values + ")";
    qDebug() << q;
    if(query.exec(q)){
        return true;
    }
    return false;
}
double DatabaseManager::getRoomCost(QString roomNo){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT cost from Space WHERE SpaceId='" + roomNo + "'";

    if(query.exec(q)){
        query.seek(0, false);
        return query.record().value(0).toString().toDouble();
    }
    return -1;
}

int DatabaseManager::getMonthlyRate(QString room, QString program){
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM CLIENT WHERE ClientId ='" + id + "'";
    if(!query.exec(q)){
        qDebug() << "ERROR IN PULLCLIENT";
    }
    return query;
}
bool DatabaseManager::updateBalance(double d, QString id){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "UPDATE Client SET Balance = '" + QString::number(d, 'f', 2)
            + "' WHERE ClientId ='" + id + "'";
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;
}
QSqlQuery DatabaseManager::getNextBooking(QDate endDate, QString roomId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT * FROM Booking WHERE StartDate >= '" + endDate.toString(Qt::ISODate) + "' AND SpaceId = '" + roomId + "' ORDER BY StartDate ASC";
    query.exec(q);
    qDebug() << q;
    return query;
}

QSqlQuery DatabaseManager::getCurrentBooking(QDate start, QDate end, QString program){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "SELECT Space.SpaceId, Space.SpaceCode, Space.ProgramCodes, Space.type, Space.cost, Space.Monthly FROM Space"
                " LEFT JOIN (SELECT * from Booking WHERE StartDate <= '" + end.toString(Qt::ISODate) + "' AND EndDate > '"
                + start.toString(Qt::ISODate) + "') AS a on Space.SpaceId = a.SpaceId WHERE BookingID IS NULL AND Space.ProgramCodes LIKE '%" + program + "%'";

    qDebug() << q;
    query.exec(q);
    return query;
}
bool DatabaseManager::updateBooking(QString q){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    qDebug() << q;
    if(query.exec(q))
        return true;
    return false;
}

bool DatabaseManager::insertBookingTable(QString insert){
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    query.exec("SELECT TOP 1 Role FROM Employee WHERE Username='" + username + "' AND Password='" + password + "'");
    return query;
}

QSqlQuery DatabaseManager::findUser(QString username) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    query.exec("SELECT Username FROM Employee WHERE Username='" + username + "'");
    return query;
}

QSqlQuery DatabaseManager::addNewEmployee(QString username, QString password, QString role, QString name) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("INSERT INTO Employee VALUES ('" + username + "', '" + password + "', '" + role + "', '" + name + "')");

    return query;
}

QSqlQuery DatabaseManager::updateEmployee(QString username, QString password, QString role, QString name) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("UPDATE Employee SET EmpName='"+ name +"', Password='" + password + "', Role='" + role + "'WHERE Username='" + username + "';");

    return query;
}

QSqlQuery DatabaseManager::deleteEmployee(QString username, QString password, QString role) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("DELETE FROM Employee WHERE Username='" + username
               + "' AND Password='" + password + "' AND Role='" + role + "';");

    return query;
}


QSqlQuery DatabaseManager::getActiveBooking(QString user, bool userLook){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString date = QDate::currentDate().toString(Qt::ISODate);
    QString q;
    if(!userLook){
         q = "SELECT * FROM Booking JOIN Space on Booking.SpaceId = Space.SpaceId WHERE FirstBook = 'YES' AND EndDate >= '" + date + "' AND StartDate != EndDate ORDER BY ClientName ASC";
    }
    else{
         q = "SELECT * FROM Booking JOIN Space on Booking.SpaceId = Space.SpaceId WHERE FirstBook = 'YES' AND EndDate >= '"
                 + date + "' AND ClientName LIKE '%" + user + "%' AND StartDate != EndDate ORDER BY ClientName ASC";

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
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("INSERT INTO Program VALUES('" + pcode +"', '" + pdesc + "')");

    return query;
}

QSqlQuery DatabaseManager::getAvailableBeds(QString pcode) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly, s.ProgramCodes "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE s.ProgramCodes NOT LIKE '%" + pcode + "%' ORDER BY s.SpaceCode");

    // qDebug() << ":" + pcode+ ":";

    return query;
}

QSqlQuery DatabaseManager::getAssignedBeds(QString pcode) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly, s.ProgramCodes "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE s.ProgramCodes LIKE '%" + pcode + "%' ORDER BY s.SpaceCode");

    return query;
}

QSqlQuery DatabaseManager::searchSingleBed(QString buildingno, QString floorno, QString roomno, QString spaceno) {
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("SELECT r.RoomId, b.BuildingId, f.FloorId "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE b.BuildingNo =" + buildingno + " "
               "AND f.FloorNo =" + floorno + " "
               "AND r.RoomNo =" + roomno + " ");

    qDebug() << "AIYIAAA " << "SELECT r.RoomId, b.BuildingId, f.FloorId "
                "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
                "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
                "WHERE b.BuildingNo =" + buildingno + " "
                "AND f.FloorNo =" + floorno + " "
                "AND r.RoomNo =" + roomno + " ";

    return query;
}

QSqlQuery DatabaseManager::deleteSpace(QString buildingno, QString floorno, QString roomno, QString spaceno) {
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
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
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("UPDATE Program SET Description='" + pdesc + "' WHERE ProgramCode='" + pcode + "'");

    return query;
}

QSqlQuery DatabaseManager::updateSpaceProgram(QString spaceid, QString program) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("UPDATE Space SET ProgramCodes='" + program + "' WHERE SpaceId=" + spaceid);

    return query;
}
bool DatabaseManager::updateLunchRoom(QDate startDate, QDate endDate, QString clientId, QString rooomId){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString q = "UPDATE Lunches SET SpaceCode ='" + rooomId +"' WHERE LunchDate >= '" + startDate.toString(Qt::ISODate) + "' AND LunchDate <= '" + endDate.toString(Qt::ISODate)
            + "' AND ClientId = '" + clientId +"'";
    qDebug() <<q;
    return query.exec(q);
}

QSqlQuery DatabaseManager::addPcp(int rowId, QString clientId, QString type, QString goal, QString strategy, QString date) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery checkQuery(db);
    QSqlQuery dataQuery(db);
    QString checkRowId = "SELECT COUNT(*) FROM Pcp "
                         "WHERE ClientId = " + clientId +
                         " AND Type = '" + type + "'"
                         " AND rowId = " + QString::number(rowId);
    checkQuery.exec(checkRowId);
    qDebug() << checkRowId;
    checkQuery.next();
    qDebug() << "rows found: " << checkQuery.value(0).toString();
    if (checkQuery.value(0).toString() == "0") {
        qDebug() << "inserting new row";
        dataQuery.prepare("INSERT INTO Pcp (rowId, clientId, Type, Goal, Strategy, Date) "
                      "VALUES (?, ?, ?, ?, ? ,?)");
        dataQuery.addBindValue(QString::number(rowId));
        dataQuery.addBindValue(clientId);
        dataQuery.addBindValue(type);
        dataQuery.addBindValue(goal);
        dataQuery.addBindValue(strategy);
        dataQuery.addBindValue(date);
        dataQuery.exec();

        return dataQuery;
    }

    dataQuery.prepare("UPDATE Pcp SET Goal=:goal, Strategy=:strategy, Date=:date WHERE Type=:type AND clientId=:clientId AND rowId=:rowId");
    qDebug() << "updating existing row";
    dataQuery.bindValue(":goal", goal);
    dataQuery.bindValue(":strategy", strategy);
    dataQuery.bindValue(":date", date);
    dataQuery.bindValue(":type", type);
    dataQuery.bindValue(":clientId", clientId);
    dataQuery.bindValue(":rowId", rowId);

    dataQuery.exec();

    return dataQuery;
}

QSqlQuery DatabaseManager::addNote(QString clientId, QString notes) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.prepare("INSERT INTO RunningNotes (ClientId, Notes) "
                  "VALUES (?, ?)");

    query.addBindValue(clientId);
    query.addBindValue(notes);

    query.exec();

    return query;
}

QSqlQuery DatabaseManager::updateNote(QString clientId, QString notes) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.prepare("UPDATE RunningNotes SET Notes=:notes WHERE ClientId=:client");

    query.bindValue(":notes", notes);
    query.bindValue(":client", clientId);

    query.exec();

    return query;
}

QSqlQuery DatabaseManager::readNote(QString clientId) {
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("SELECT Notes FROM RunningNotes WHERE ClientId = " + clientId);

    return query;
}


QSqlQuery DatabaseManager::getProgramDesc(QString programcode){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);

    query.exec("SELECT Description FROM Program WHERE ProgramCode = '" + programcode + "'");
    qDebug() << "SELECT Description FROM Program WHERE ProgramCode = '" + programcode + "'";
    return query;

}

//SHIFT QUERY
bool DatabaseManager::updateShift(bool shiftExist, QString selectedDay, QStringList *shiftList){
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString queryStr;
    if(!shiftExist)
        queryStr = "INSERT INTO Shift VALUES(?,?,?, ?,?,?,?,?,?,?,?,?)";
    else{

        queryStr = "UPDATE Shift SET ";
        for (int i =1; i <6; i++){
            queryStr += QString("StartTimeShift"+QString::number(i)+" = ?, EndTimeShift"+QString::number(i)+" = ?, ");

        }
        queryStr += QString("NumShifts = ? WHERE DayOfWeek = '" + selectedDay +"'");
    }
    query.prepare(queryStr);
    qDebug()<<"UPDATE SHIFT "<< queryStr;
    for (int i = 0; i < shiftList->size(); ++i)
    {
        if (shiftList->at(i) != NULL)
        {
            qDebug()<<"["<<i<<"] : "<<shiftList->at(i);
            query.addBindValue(shiftList->at(i));
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

QSqlQuery DatabaseManager::getShiftInfoDaily(QString day)
{
    DatabaseManager::checkDatabaseConnection(&db);
    QSqlQuery query(db);
    QString queryStr = "SELECT * FROM Shift ";
    if(day != "")
        queryStr += "WHERE DayOfWeek = '" + day + "'";
    query.exec(queryStr);
    return query;
}

void DatabaseManager::readPcpThread(QString clientId, QString type, int idx)
{
    QString connName = QString::number(dbManager->getDbCounter());
    QStringList goal, strategy, date;
    bool conn = true;
    {
        QSqlDatabase tempDb = QSqlDatabase::database();

        if (dbManager->createDatabase(&tempDb, connName))
        {
            QSqlQuery query(tempDb);
            if (DatabaseManager::getPcpQuery(&query, clientId, type))
            {
                while (query.next())
                {
                    goal << query.value(1).toString();
                    strategy << query.value(2).toString();
                    date << query.value(3).toString();
                }
            }
            tempDb.close();
        }
        else
        {
            tempDb.close();
            conn = false;
        }
    } // Necessary braces: tempDb and query are destroyed because out of scope
    QSqlDatabase::removeDatabase(connName);

    emit DatabaseManager::getPcpData(goal, strategy, date, idx, conn);
}

bool DatabaseManager::getPcpQuery(QSqlQuery* query, QString curClientID, QString type) {
    QString result = "SELECT rowId, Goal, Strategy, Date "
                     "FROM Pcp "
                     "WHERE ClientId = " + curClientID +
                     " AND Type = '" + type + "'";
    return query->exec(result);
}
