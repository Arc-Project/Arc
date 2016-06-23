#include "duplicateclients.h"
#include "ui_duplicateclients.h"
#include <QDebug>

DuplicateClients::DuplicateClients(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DuplicateClients)
{
    ui->setupUi(this);
}

DuplicateClients::~DuplicateClients()
{
    delete ui;
}

void DuplicateClients::displayList(QSqlQuery infoResults){
    qDebug()<<"display result";
    int row =0;
    while(infoResults.next()){
        ui->tableWidget_same_name->insertRow(row);
        for(int i =0; i<6; i++){
            ui->tableWidget_same_name->setItem(row, i, new QTableWidgetItem(infoResults.value(i).toString()));
//            qDebug() <<"row : "<<row << ", col: " << i << "item" << infoResults.value(i).toString();
        }
        row++;
    }
    ui->tableWidget_same_name->setColumnHidden(6, true);

}


void DuplicateClients::on_pushButton_cl_warning_Select_clicked()
{

    qDebug()<<"ROW NUM " <<nRow;
    if(nRow < 0){
        ui->label_warning_msg->setText("Please Select Client");
    }
    else{
        QString clientId = ui->tableWidget_same_name->item(nRow, 5)->text();
        emit selectedUser(clientId);
        close();
    }
}

void DuplicateClients::on_tableWidget_same_name_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    nRow = row;
}

void DuplicateClients::on_pushButton_cl_warning_ignore_clicked()
{
    qDebug() << "ignore clicked";
    emit ignoreWarning();
    close();
}
