#include "payment.h"
#include "ui_payment.h"
#include <QDebug>
#include "databasemanager.h"
#include <QMessageBox>
payment::payment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);

}
payment::payment(QWidget *parent, transaction * trans, double balance, double cost, Client * client, QString note, bool paymentType, QString emp, QString shift ) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    transact = trans;
    hideCheque();
    this->client = client;
    ui->chequeDate->setDate(QDate::currentDate());
    ui->balanceLabel->setText(QString::number(balance));
    ui->transactionLabel->setText(QString::number(cost));
    ui->owedLabel->setText(QString::number(cost - balance));
    ui->plainTextEdit->insertPlainText(note);
    shiftNo = shift;
    empId = emp;
    if(paymentType){
        doPayment();
        ui->paymentRadio->setChecked(true);
    }
    else{
        ui->refundRadio->setChecked(true);
        doRefund();
    }
}


payment::~payment()
{
    delete ui;
}
bool payment::makePayment(){
    QString values;
    if(ui->paymentRadio->isChecked()){
        if(ui->paymentDrop->currentIndex() == 1){


            values = "'" + client->clientId + "','" + QDate::currentDate().toString(Qt::ISODate)
                    + "','" + QString::number(transact->amount) + "','" + transact->type + "','" + transact->notes
                    + "','" + transact->chequeNo + "','" + transact->MSQ + "','" + transact->issuedString + "','" + transact->transType
                    + "', 0" + ",'" + transact->outstanding + "', '" + empId + "','" + shiftNo + "','" + QTime::currentTime().toString() + "'";
        }
        else{
        values = "'" + client->clientId + "','" + QDate::currentDate().toString(Qt::ISODate)
                + "','" + QString::number(transact->amount) + "','" +  transact->type + "','" + transact->notes
                + "', NULL, NULL, NULL" + ",'" + transact->transType + "', 0" + ",'" + transact->outstanding + "', '" + empId + "','" + shiftNo + "','" + QTime::currentTime().toString() + "'";
        }
    }
    else{
        values = "'" + client->clientId + "','" + QDate::currentDate().toString(Qt::ISODate)
                + "','" + QString::number(transact->amount) + "','" + transact->type + "','" + transact->notes
                + "', NULL, NULL, NULL" + ",'" + transact->transType + "', 0" + ",'" + transact->outstanding + "', '" + empId + "','" + shiftNo + "','" + QTime::currentTime().toString() + "'";

    }
    if(dbManager->addPayment(values)){
        return true;

    }
    return false;
}
bool payment::addTransaction(){
    QString type;
    ui->paymentRadio->isChecked() == true ? type = "Payment" : type = "Refund";
    if(!checkNumber(ui->paymentInput->text()))
        return false;
    transact->chequeNo = "NULL";
    transact->notes = "NONE";
    transact->MSQ = "NO";\
    if(ui->paymentDrop->currentIndex() == 1){
        transact->issuedString = ui->chequeDate->date().toString(Qt::ISODate);
    }

    double amt;
    /*if(!(amt = ui->paymentInput->text().toDouble())){
        qDebug() << "Invalid string";
    }
    else{
        transact->amount = amt;
    }*/
    if(ui->payWaitMSD->isChecked()){

        transact->outstanding = "1";
    }
    else{
        transact->outstanding= "0";
    }
    transact->amount = ui->paymentInput->text().toDouble();
    transact->type = ui->paymentDrop->currentText();
    transact->notes = ui->plainTextEdit->toPlainText();
    if(ui->msqCheck->isChecked()){
        transact->MSQ = "YES";
    }

    transact->issued = ui->chequeDate->date();
    transact->today = QDate::currentDate();
    transact->chequeNo = ui->chequeInput->text();
    transact->transType = type;
    //CHANGE THIS
    transact->empId = "1";
    return true;
}

void payment::on_paymentBox_accepted()
{

}
bool payment::doMessageBox(QString message){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", message, QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes){
        return true;
    }
    return false;

}

void payment::on_addPaymentButton_clicked()
{
    if(!addTransaction())
        return;
    double newBal;
    if(ui->paymentRadio->isChecked()){
        if(ui->payWaitMSD->isChecked())
            newBal = client->balance;
        else
            newBal = client->balance + transact->amount;
    }
    else{
        newBal = client->balance - transact->amount;
    }
    if(!dbManager->updateBalance(newBal, client->clientId)){
        return;
    }
    else{
        client->balance = newBal;
    }
    if(makePayment()){
//        ui->addPaymentButton->setEnabled(false);
    }

}
bool payment::checkNumber(QString num){

    int l = num.length();
    if(l > 8)
        return false;
    int period = 0;
    char copy[l];
    strcpy(copy, num.toStdString().c_str());
    for(int i = 0; i < num.length(); i++){
        if(copy[i] == '.'){
            if(period)
                return false;
            period++;
            continue;
        }
        /*if(copy[i] == '-'){
            if(i == 0)
                continue;
             return false;
        }
*/
        if(!isdigit(copy[i]))
            return false;
    }
    return true;
}

void payment::on_paymentDrop_currentIndexChanged(const QString &arg1)
{
    qDebug() << arg1;
    if(ui->paymentDrop->currentIndex() == 1){
       showCheque();
    }
    else{
       hideCheque();
    }
}
void payment::showCheque(){
//    ui->chequeDate->show();
//    ui->chequeInput->show();
//    ui->msqCheck->show();
//    ui->chequeLabel->show();
//    ui->payWaitMSD->show();
//    ui->dateLabel->show();

    ui->chequeDate->setEnabled(true);
    ui->chequeInput->setEnabled(true);
    ui->msqCheck->setEnabled(true);
    ui->chequeLabel->setEnabled(true);
    ui->payWaitMSD->setEnabled(true);
    ui->dateLabel->setEnabled(true);
}
void payment::hideCheque(){
//    ui->chequeDate->hide();
//    ui->chequeLabel->hide();
//    ui->dateLabel->hide();
//    ui->chequeInput->hide();
//    ui->payWaitMSD->hide();
//    ui->msqCheck->hide();

    ui->chequeDate->setEnabled(false);
    ui->chequeLabel->setEnabled(false);
//    ui->dateLabel->setEnabed(false);
    ui->chequeInput->setEnabled(false);
    ui->payWaitMSD->setEnabled(false);
    ui->msqCheck->setEnabled(false);

    ui->chequeInput->setText("");
    ui->msqCheck->setChecked(false);
    ui->payWaitMSD->setChecked(false);
}
void payment::doPayment(){
    ui->paymentDrop->setEnabled(true);
    ui->payRefLabel->setText("Payment Amount");
//    ui->addPaymentButton->setText("Add Payment");

}
void payment::doRefund(){
    ui->payRefLabel->setText("Refund Amount");
    ui->paymentDrop->setEnabled(false);
//    ui->addPaymentButton->setText("Add Refund");
    hideCheque();

}

void payment::on_paymentRadio_clicked()
{
    if(!ui->paymentRadio->isChecked()){
        return;
    }
    doPayment();
}

void payment::on_refundRadio_clicked()
{
    if(!ui->refundRadio->isChecked()){
        return;
    }
    doRefund();
}
bool payment::checkQuote(QString text){
    const char * test = text.toStdString().c_str();
    for(int i = 0; i < text.length(); i++){
        if(test[i] == '\'')
            return false;
    }
    return true;
}

void payment::on_btn_ok_clicked()
{
    if(!checkNumber(ui->paymentInput->text()))
        return;
    if(!checkQuote(ui->plainTextEdit->toPlainText())){
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("ArcWay");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText("Single quotes (') not allowed in note");
        msgBox.exec();

        return;
    }
    if(doMessageBox("Adding payment, this is permentant, confirm?")){

        payment::on_addPaymentButton_clicked();
        close();
    }
    else{
        return;
    }
}
