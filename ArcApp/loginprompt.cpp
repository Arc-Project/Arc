#include "loginprompt.h"
#include "mainwindow.h"
#include "ui_loginprompt.h"
#include "worker.h"
LoginPrompt::LoginPrompt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginPrompt)
{
    ui->setupUi(this);
    
    ui->label_LoginError->setText(DatabaseManager::staticError);
}

LoginPrompt::~LoginPrompt()
{
    delete ui;
}

// login button
void LoginPrompt::on_btn_login_clicked()
{
    QSqlQuery queryResults;
    bool validLogin = false;

    QString username = ui->lineEdit_Uname->text();
    QString pw = ui->lineEdit_Pw->text();

    if (username.isEmpty()) {
        ui->label_LoginError->setText("Please enter a username");
    } else if (pw.isEmpty()) {
        ui->label_LoginError->setText("Please enter a password");
    } else {
        queryResults = dbManager->loginSelect(username, pw);

        int numrows = queryResults.numRowsAffected();

        if (numrows > 0) {
            validLogin = true;
            qDebug("valid login");
        } else {
            validLogin = false;
            qDebug("invalid login");
            ui->label_LoginError->setText("Invalid login credentials");
        }
    }

    if (validLogin) {
        MainWindow* w = new MainWindow();
        w->show();

        w->userLoggedIn = username;


        w->updatemenuforuser();

        // thread for updating shift no every minute
        w->work = new Worker();
        w->thread = new QThread;
        w->work->moveToThread(w->thread);
        QObject::connect(w->thread, SIGNAL(started()), w->work, SLOT(process()));
        QObject::connect(w->work, SIGNAL(shiftnochanged()), w, SLOT(setShift()));
        QObject::connect(w->work, SIGNAL(finished()), w->thread, SLOT(quit()));
        QObject::connect(w->work, SIGNAL(finished()), w->work, SLOT(deleteLater()));
        QObject::connect(w->thread, SIGNAL(finished()), w->thread, SLOT(deleteLater()));
        w->thread->start();

        close();
    }
}

void LoginPrompt::on_btn_cancelLogin_clicked()
{
    close();
}
