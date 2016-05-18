#ifndef PAYMENT_H
#define PAYMENT_H
#include <QDialog>
#include "transaction.h"
#include "client.h"
#include "booking.h"

namespace Ui {
class payment;
}

class payment : public QDialog
{
    Q_OBJECT

public:
    explicit payment(QWidget *parent = 0);
    payment(QWidget *parent, transaction * trans, double balance, double cost, Client * client, QString note, bool paymentType, QString emp, QString shift);
    ~payment();
    bool makePayment();
    bool checkNumber(QString num);
private slots:
    void on_paymentBox_accepted();
    bool addTransaction();
    void on_addPaymentButton_clicked();
    void hideCheque();
    void showCheque();
    void on_paymentDrop_currentIndexChanged(const QString &arg1);
    void doPayment();
    void doRefund();
    void on_paymentRadio_clicked();
    bool doMessageBox(QString message);
    bool checkQuote(QString text);
    void on_refundRadio_clicked();

    void on_btn_ok_clicked();

private:
    Ui::payment *ui;
    transaction * transact;
    Client * client;
    Booking * curBook;
    QString empId;
    QString shiftNo;
};

#endif // PAYMENT_H
