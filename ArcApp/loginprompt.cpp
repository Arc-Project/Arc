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
    //qDebug("SDfSDF");

    bool validLogin = false;

    QString username = ui->lineEdit_Uname->text();
    QString pw = ui->lineEdit_Pw->text();

    if (username.isEmpty()) {
        ui->label_LoginError->setText("Please enter a username");
    } else if (pw.isEmpty()) {
        ui->label_LoginError->setText("Please enter a password");
    } else {
        bool validCredentials = true; // true for testing
        // validation - go to database and check uname and pw; set validLogin here later and remove validCredentials


        if (!validCredentials) {
            ui->label_LoginError->setText("Invalid login credentials");
        } else {
            // temp for testing
            validLogin = true;
        }
    }

    if (validLogin) {
        MainWindow* w = new MainWindow();
        w->show();
        close();
    }
}

