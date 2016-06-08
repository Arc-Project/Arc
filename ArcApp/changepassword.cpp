#include "changepassword.h"
#include "ui_changepassword.h"
#include <QDebug>

ChangePassword::ChangePassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePassword)
{
    ui->setupUi(this);
}

ChangePassword::~ChangePassword()
{
    delete ui;
}

void ChangePassword::on_pushButton_userpw_ok_clicked()
{
    if(!confirm_Password()){
        ui->label_userpw_msg->setText("Password is not matched");
    }
    else{
        emit newPw(ui->lineEdit_userpw_pw->text());
        close();
    }
}



bool ChangePassword::confirm_Password(){
    qDebug()<<"password1 "<< ui->lineEdit_userpw_pw->text();
    qDebug()<<"password2 "<< ui->lineEdit_userpw_confirm->text();
    if(ui->lineEdit_userpw_pw->text() == ui->lineEdit_userpw_confirm->text())
        return true;
    qDebug()<<"NOTMATCHED";
    return false;
}


void ChangePassword::on_pushButton_userpw_cancel_clicked()
{
     close();
}
