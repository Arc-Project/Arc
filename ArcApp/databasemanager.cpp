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

QSqlQuery DatabaseManager::loginSelect(QString username, QString password) {
    QSqlQuery query(db);
    query.exec("SELECT TOP 1 Role FROM Employee WHERE Username='" + username + "' AND Password='" + password + "'");
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

bool DatabaseManager::uploadCaseFile(QString filepath)
{
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
    qDebug() << "Filename:\t\t" <<fileInfo.baseName();
    qDebug() << "Extension:\t\t" <<fileInfo.suffix();
    qDebug() << "Size:\t\t\t" <<fileInfo.size();
    byte = file.readAll();
    file.close();

    QSqlQuery query(db);
    query.prepare("INSERT INTO FileTest1(doc, fileName, extension, fileSize) VALUES(:doc, :fname, :fext, :fsize)");
    query.bindValue(":doc", byte, QSql::In | QSql::Binary);
    query.bindValue(":fname", fileInfo.fileName());
    query.bindValue(":fext", fileInfo.suffix());
    query.bindValue(":fsize", fileInfo.size());

//    if (query.exec())
//    {
//        return true;
//    }

      DatabaseManager::downloadLatestCaseFile();

      return true;
//    return false;
}

QSqlQuery DatabaseManager::getLatestFileUploadEntry(QString tableName)
{
    QSqlQuery query(db);
    query.exec("SELECT TOP 1 * FROM " + tableName + " ORDER BY id DESC");
    return query;
}

bool DatabaseManager::downloadLatestCaseFile()
{
    QSqlQuery queryResults = DatabaseManager::getLatestFileUploadEntry("FileTest1");
    queryResults.next();
    QString filename = queryResults.value(3).toString();
    QByteArray data = queryResults.value(2).toByteArray();
    qDebug() << filename;

    QFile file("..\\Downloads\\" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}
