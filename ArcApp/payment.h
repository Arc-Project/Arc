#ifndef PAYMENT_H
#define PAYMENT_H
#include <QDialog>
#include "transaction.h"
#include "client.h";
#include "booking.h"

namespace Ui {
class payment;
}

class payment : public QDialog
{
    Q_OBJECT

public:
    explicit payment(QWidget *parent = 0);
    payment(QWidget *parent, transaction * trans, double balance, double cost, Client * client, Booking * book);
    ~payment();
    bool makePayment();
private slots:
    void on_paymentBox_accepted();
    void addTransaction();
    void on_addPaymentButton_clicked();

private:
    Ui::payment *ui;
    transaction * transact;
    Client * client;
    Booking * curBook;
};

#endif // PAYMENT_H
