#include "mainwindow.h"
#include <QApplication>

DatabaseManager* dbManager;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    dbManager = new DatabaseManager();
    w.show();

    return a.exec();
}
