#include "mainwindow.h"
#include "loginprompt.h"
#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <worker.h>

DatabaseManager* dbManager;

QThread* MainWindow::thread = new QThread;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dbManager = new DatabaseManager();
    //MainWindow w;

    // thread for updating shift no every minute
    Worker* worker = new Worker();
    worker->moveToThread(MainWindow::thread);
//    QObject::connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    QObject::connect(MainWindow::thread, SIGNAL(started()), worker, SLOT(process()));
    QObject::connect(worker, SIGNAL(finished()), MainWindow::thread, SLOT(quit()));
    QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    QObject::connect(MainWindow::thread, SIGNAL(finished()), MainWindow::thread, SLOT(deleteLater()));
    MainWindow::thread->start();

    LoginPrompt w; //

    w.show();

    /*QFile styleFile(":qdarkstyle/style.qss");
    styleFile.open(QFile::ReadOnly | QFile::Text);
    QByteArray bt = styleFile.readAll();
    QString style(bt);
    a.setStyleSheet(style);*/

    return a.exec();
}





