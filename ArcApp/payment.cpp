#include "payment.h"
#include "ui_payment.h"
#include <QDebug>
payment::payment(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
}
payment::payment(QWidget *parent, transaction * trans, double balance, double cost) :
    QDialog(parent),
    ui(new Ui::payment)
{
    ui->setupUi(this);
    transact = trans;
    ui->balanceLabel->setText(QString::number(balance));
    ui->transactionLabel->setText(QString::number(cost));
}


payment::~payment()
{
    delete ui;
}

void payment::on_paymentBox_accepted()
{
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
    else{
        transact->MSQ = "NO";
    }
    transact->issued = ui->chequeDate->date();
    transact->today = QDate::currentDate();
    transact->chequeNo = ui->chequeInput->text();
    //CHANGE THIS
    transact->empId = "1";
}
