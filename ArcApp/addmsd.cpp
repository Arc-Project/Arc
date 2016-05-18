#include "addmsd.h"
#include "ui_addmsd.h"

AddMSD::AddMSD(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddMSD)
{
    ui->setupUi(this);
}
AddMSD::AddMSD(QWidget *parent, transaction * trans) :
    QDialog(parent),
    ui(new Ui::AddMSD)
{
    transac = trans;
    ui->setupUi(this);
}
AddMSD::~AddMSD()
{
    delete ui;
}

void AddMSD::on_pushButton_clicked()
{
    transac->chequeNo = ui->msdChequeNum->text();
    close();
}

void AddMSD::on_pushButton_2_clicked()
{
    transac->chequeNo = "NO";
    close();
}
