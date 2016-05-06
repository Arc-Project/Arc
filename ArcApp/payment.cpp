#include "payment.h"
#include "ui_payment.h"
#include <QDebug>
#include "databasemanager.h"
payment::payment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
}
payment::payment(QWidget *parent, transaction * trans, double balance, double cost, Client * client ) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    transact = trans;
    this->client = client;
    ui->balanceLabel->setText(QString::number(balance));
    ui->transactionLabel->setText(QString::number(cost));
}


payment::~payment()
{
    delete ui;
}
bool payment::makePayment(){
    QString values;

    values = "'" + client->clientId + "','" + QDate::currentDate().toString(Qt::ISODate)
            + "','" + QString::number(transact->amount) + "','" + "6" + "','" + transact->type + "','" + transact->notes
            + "','" + transact->chequeNo + "','" + transact->MSQ + "'";
    if(dbManager->addPayment(values))
        return true;
    return false;
}
void payment::addTransaction(){
    transact->chequeNo = "NULL";
    transact->notes = "NONE";
    transact->MSQ = "NO";
    double amt;
    if(!(amt = ui->paymentInput->text().toDouble())){
        qDebug() << "Invalid string";
    }
    else{
        transact->amount = amt;
    }
    transact->type = ui->paymentDrop->currentText();
    transact->notes = ui->plainTextEdit->toPlainText();
    if(ui->msqCheck->isChecked()){
        transact->MSQ = "YES";
    }

    transact->issued = ui->chequeDate->date();
    transact->today = QDate::currentDate();
    transact->chequeNo = ui->chequeInput->text();
    //CHANGE THIS
    transact->empId = "1";
}

void payment::on_paymentBox_accepted()
{
    qDebug() << "Anthony hates his project";
}

void payment::on_addPaymentButton_clicked()
{
    addTransaction();
    if(makePayment()){
        ui->addPaymentButton->setEnabled(false);
    }

}
