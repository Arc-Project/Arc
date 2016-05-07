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
payment::payment(QWidget *parent, transaction * trans, double balance, double cost, Client * client, Booking * book ) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    transact = trans;
    hideCheque();
    curBook = book;
    this->client = client;
    ui->chequeDate->setDate(QDate::currentDate());
    ui->balanceLabel->setText(QString::number(balance));
    ui->transactionLabel->setText(QString::number(cost));
    ui->owedLabel->setText(QString::number(cost - balance));
}


payment::~payment()
{
    delete ui;
}
bool payment::makePayment(){
    QString values;
    if(ui->paymentDrop->currentIndex() == 1){
        values = "'" + client->clientId + "','" + QDate::currentDate().toString(Qt::ISODate)
                + "','" + QString::number(transact->amount) + "','" + "6" + "','" + transact->type + "','" + transact->notes
                + "','" + transact->chequeNo + "','" + transact->MSQ + "','" + transact->issuedString + "'";
    }
    else{
    values = "'" + client->clientId + "','" + QDate::currentDate().toString(Qt::ISODate)
            + "','" + QString::number(transact->amount) + "','" + "6" + "','" + transact->type + "','" + transact->notes
            + "', NULL, NULL, NULL";
    }
    if(dbManager->addPayment(values)){
        curBook->paidTotal+= transact->amount;
        return true;

    }
    return false;
}
void payment::addTransaction(){
    transact->chequeNo = "NULL";
    transact->notes = "NONE";
    transact->MSQ = "NO";\
    if(ui->paymentDrop->currentIndex() == 1){
        transact->issuedString = ui->chequeDate->date().toString(Qt::ISODate);
    }

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
    ui->chequeDate->show();
    ui->chequeInput->show();
    ui->msqCheck->show();
    ui->chequeLabel->show();
    ui->dateLabel->show();
}
void payment::hideCheque(){
    ui->chequeDate->hide();
    ui->chequeLabel->hide();
    ui->dateLabel->hide();
    ui->chequeInput->hide();
    ui->msqCheck->hide();
    ui->chequeInput->setText("");
    ui->msqCheck->setChecked(false);
}
