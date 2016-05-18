#include "mainwindow.h"
#include "loginprompt.h"
#include <QApplication>
#include <QFile>
#include <QByteArray>

DatabaseManager* dbManager;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dbManager = new DatabaseManager();
    MainWindow w;
    //LoginPrompt w; //

    w.show();

    // make thread to update shift no


    /*QFile styleFile(":qdarkstyle/style.qss");
    styleFile.open(QFile::ReadOnly | QFile::Text);
    QByteArray bt = styleFile.readAll();
    QString style(bt);
    a.setStyleSheet(style);*/

    return a.exec();
}





