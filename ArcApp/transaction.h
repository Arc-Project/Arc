#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <QDate>

class transaction
{
public:
    transaction();
    double amount;
    QDate today;
    QDate issued;
    QString empId;
    QString type;
    QString notes;
    QString chequeNo;
    QString MSQ;
};

#endif // TRANSACTION_H
