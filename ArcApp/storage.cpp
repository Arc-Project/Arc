#include "storage.h"
#include "ui_storage.h"
#include <QMessageBox>
Storage::Storage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Storage)
{
    ui->setupUi(this);
}

Storage::Storage(QWidget *parent, QString clientId, QString clientName, QString prev, QString storageId) :
    QDialog(parent),
    ui(new Ui::Storage)
{
    newStore = true;
    id = clientId;
    name = clientName;
    ui->setupUi(this);
    if(prev == ""){

        loadPrevious();
    }
    else{
        ui->storageInput->insertPlainText(prev);
        storeId = storageId;
        newStore = false;
    }
}

Storage::~Storage()
{
    delete ui;
}
bool Storage::doMessageBox(QString message){
    
    QString tmpStyleSheet = parentWidget()->styleSheet();
    parentWidget()->setStyleSheet("");
    
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", message, QMessageBox::Yes | QMessageBox::No);
    
    parentWidget()->setStyleSheet(tmpStyleSheet);
    
    if(reply == QMessageBox::Yes){
        return true;
    }
    return false;

}
void Storage::loadPrevious(){
    QSqlQuery result = dbManager->loadStorage(id);
    if(result.next()){
        newStore = false;
        storeId = result.value("StorageId").toString();
        QString prev  = result.value("StorageItems").toString();
        ui->storageInput->insertPlainText(prev);
    }
}

void Storage::on_storageOk_clicked()
{
    if(!doMessageBox("Update storage?"))
        return;
    QString storeInfo = ui->storageInput->toPlainText();
    if(newStore){
        if(!dbManager->addStorage(id, name, storeInfo))
            return;
        QSqlQuery result = dbManager->loadStorage(id);
        if(!result.next())
            return;
        storeId = result.value("StorageId").toString();
        if(!dbManager->updateStore(storeId, storeInfo))
            return;
    }else{
        if(!dbManager->updateStore(storeId, storeInfo))
            return;
    }
    if(!dbManager->updateStoreDate(storeId))
        return;

    close();


}

void Storage::on_storageCancel_clicked()
{
    close();
}
