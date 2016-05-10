#include "databasemanager.h"
#include "shared.h"


int DatabaseManager::dbCounter = 0;
QMutex DatabaseManager::mutex;

/*==============================================================================
DATABASE MANAGER SETUP (START)
==============================================================================*/
/*
 * Constuctor
 * Sets up the default database connection.
 */
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
bool DatabaseManager::searchClientList(QSqlQuery* query, QString ClientId){
    query =  new QSqlQuery(db);
    query->prepare("SELECT FirstName, MiddleName, LastName, Dob FROM Client WHERE ClientId = :id");
    query->bindValue("id", ClientId);

    if(query->exec())
        return true;
    return false;
}

QSqlQuery DatabaseManager::searchClientInfo(QSqlQuery* selectquer, QString ClientId){
    QSqlQuery selectquery(db);
    selectquery.prepare("SELECT FirstName, MiddleName, LastName, Dob, Balance, SinNo, GaNo, IsParolee, AllowComm, DateRulesSigned FROM Client WHERE ClientId = :id");
    selectquery.bindValue("id", ClientId);

    selectquery.exec();

    printAll(selectquery);
    return selectquery;
    /*
    if(!selectquery->exec())
        return false;
*/

    /*
    printAll(*query);
    QSqlQuery test(db);
    qDebug()<<"TEST: Query";
    test.exec("SELECT FirstName, MiddleName, LastName, Dob, Balance, SinNo, GaNo, IsParolee, AllowComm, DateRulesSigned FROM Client WHERE ClientId = "+ClientId);
    printAll(test);
    */
    //return true;
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

bool DatabaseManager::insertClientWithPic(QStringList* registerFieldList, QImage* profilePic)//QObject sth, QImage profilePic)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    profilePic->save(&buffer, "PNG");  // writes image into ba in PNG format

    QSqlQuery query(db);

    query.prepare(QString("INSERT INTO Client ")
        + QString("(FirstName, MiddleName, LastName, Dob, SinNo, GaNo, EmpId, ")
        + QString("IsParolee, AllowComm, DateRulesSigned, NokName, NokRelationship, ")
        + QString("NokLocation, NokContactNo, PhysName, PhysContactNo, MsdWorkerName, ")
        + QString("MsdWorkerContactNo, Status, Comments, ProfilePic) ")
        + QString("VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"));

    for (int i = 0; i < registerFieldList->size(); ++i)
    {
        if (registerFieldList->at(i) != NULL)
        {
            qDebug()<<"["<<i<<"] : "<<registerFieldList->at(i);
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
    QString q = "SELECT * FROM Transac WHERE Date >= '" + start.toString(Qt::ISODate) + "' AND Date <= '"
            + end.toString(Qt::ISODate) + "'";
    qDebug() << q;
    query.exec(q);
    return query;
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
bool DatabaseManager::getCheckoutQuery(QSqlQuery* queryResults, QDate date)
{
    *queryResults = QSqlQuery(db);

    QString queryString =
        QString("SELECT b.ClientName, b.SpaceId, b.StartDate, ")
        + QString("b.EndDate, b.ProgramCode, c.Balance ")
        + QString("FROM Booking b INNER JOIN Client c ON b.ClientId = c.ClientId ")
        + QString("WHERE EndDate = '" + date.toString(Qt::ISODate))
        + QString("' AND FirstBook = 'YES'");

        qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getVacancyQuery(QSqlQuery* queryResults, QDate date)
{
    *queryResults = QSqlQuery(db);

    QString queryString =
        QString("SELECT s.SpaceId, s.ProgramCodes ")
        + QString("FROM Space s LEFT JOIN (SELECT SpaceId, Date ")
        + QString("FROM Booking WHERE Date = '" + date.toString(Qt::ISODate))
        + QString("') as b ON s.SpaceId = b.SpaceId ")
        + QString("WHERE b.date IS NULL");

        qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getLunchQuery(QSqlQuery* queryResults, QDate date)
{
    *queryResults = QSqlQuery(db);

    QString queryString =
        QString("SELECT ClientName, SpaceId, Lunch ")
        + QString("FROM Booking ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate)) + "' AND "
        + QString("Lunch = 'YES'");

        qDebug() << queryString;
    return queryResults->exec(queryString);
}

bool DatabaseManager::getWakeupQuery(QSqlQuery* queryResults, QDate date)
{
    *queryResults = QSqlQuery(db);

    QString queryString =
        QString("SELECT ClientName, SpaceId, Wakeup ")
        + QString("FROM Booking ")
        + QString("WHERE Date = '" + date.toString(Qt::ISODate)) + "' AND "
        + QString("Wakeup <> 'NO' AND Wakeup IS NOT NULL");

        qDebug() << queryString;
    return queryResults->exec(queryString);
}
/*==============================================================================
REPORT QUERYS (END)
==============================================================================*/

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

QSqlQuery DatabaseManager::getCurrentBooking(QDate start, QDate end, QString program){
    QSqlQuery query(db);
    QString q = "SELECT Space.SpaceId, Space.Location, Space.ProgramCodes, Space.type, Space.cost, Space.Monthly FROM Space"
                " LEFT JOIN (SELECT * from Booking WHERE StartDate <= '" + start.toString(Qt::ISODate) + "' AND EndDate >= '"
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
         q = "SELECT * FROM BOOKING WHERE FirstBook = 'YES' AND EndDate >= '" + date + "'";
    }
    else{
         q = "SELECT * FROM BOOKING WHERE FirstBook = 'YES' AND EndDate >= '" + date + "' AND ClientName LIKE '%" + user + "%'";

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

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE s.ProgramCodes NOT LIKE '%" + pcode + "%'");

    //qDebug() << ":" + pcode+ ":";

    return query;
}

QSqlQuery DatabaseManager::getAssignedBeds(QString pcode) {
    QSqlQuery query(db);

    query.exec("SELECT b.BuildingNo, f.FloorNo, r.RoomNo, s.SpaceId, s.SpaceNo, s.type, s.cost, s.Monthly "
               "FROM Space s INNER JOIN Room r ON s.RoomId = r.RoomId INNER JOIN Floor f ON r.FloorId = f.FloorId "
               "INNER JOIN Building b ON f.BuildingId = b.BuildingId "
               "WHERE s.ProgramCodes LIKE '%" + pcode + "%'");

    return query;
}


QSqlQuery DatabaseManager::updateProgram(QString pcode, QString pdesc) {
    QSqlQuery query(db);

    query.exec("UPDATE Program SET Description='" + pdesc + "' WHERE ProgramCode='" + pcode + "'");

    return query;
}
