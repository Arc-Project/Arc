#ifndef PAYMENT_H
#define PAYMENT_H
#include <QDialog>
#include "transaction.h"
#include "booking.h"

namespace Ui {
class payment;
}

class payment : public QDialog
{
    Q_OBJECT

public:
    explicit payment(QWidget *parent = 0);
    payment(QWidget *parent, transaction * trans, double balance, double cost);
    ~payment();

private slots:
    void on_paymentBox_accepted();

private:
    Ui::payment *ui;
    transaction * transact;
};

#endif // PAYMENT_H
