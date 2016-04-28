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
    MainWindow* w = new MainWindow();
    w->show();


    close();
}

