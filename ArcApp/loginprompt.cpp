#include "loginprompt.h"
#include "mainwindow.h"
#include "ui_loginprompt.h"

LoginPrompt::LoginPrompt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginPrompt)
{
    ui->setupUi(this);
}

LoginPrompt::~LoginPrompt()
{
    delete ui;
}

// login button
void LoginPrompt::on_pushButton_clicked()
{
    bool validLogin = false;

    QString username = ui->lineEdit_Uname->text();
    QString pw = ui->lineEdit_Pw->text();

    if (username.isEmpty()) {
        ui->label_LoginError->setText("Please enter a username");
    } else if (pw.isEmpty()) {
        ui->label_LoginError->setText("Please enter a password");
    } else {
        QSqlQuery queryResults = dbManager->loginSelect(username, pw);

        int numrows = queryResults.numRowsAffected();

        if (numrows != 0) {
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
        close();
    }
}

