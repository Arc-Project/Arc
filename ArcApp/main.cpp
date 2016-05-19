#include "mainwindow.h"
#include "loginprompt.h"
#include <QApplication>
#include <QFile>
#include <QByteArray>
#include <worker.h>

DatabaseManager* dbManager;
// QThread* thread = new QThread;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dbManager = new DatabaseManager();
    LoginPrompt w;

    // MainWindow mw;

    // mw.hide();
    w.show();

    /*QFile styleFile(":qdarkstyle/style.qss");
    styleFile.open(QFile::ReadOnly | QFile::Text);
    QByteArray bt = styleFile.readAll();
    QString style(bt);
    a.setStyleSheet(style);*/

    return a.exec();
}





