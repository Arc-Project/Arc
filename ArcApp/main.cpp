#include "mainwindow.h"
#include "loginprompt.h"
#include <QApplication>
#include <QFile>
#include <QByteArray>
DatabaseManager* dbManager;
QSettings settings;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //LoginPrompt w; //
    dbManager = new DatabaseManager();
    QCoreApplication::setOrganizationName("The Salvation Army ARC");
    QCoreApplication::setApplicationName("ARC Manager");
    w.show();
    /*QFile styleFile(":qdarkstyle/style.qss");
    styleFile.open(QFile::ReadOnly | QFile::Text);
    QByteArray bt = styleFile.readAll();
    QString style(bt);
    a.setStyleSheet(style);*/

    return a.exec();
}





