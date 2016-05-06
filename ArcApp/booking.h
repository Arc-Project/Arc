#ifndef BOOKING_H
#define BOOKING_H

#include <QDate>
class Booking
{
public:
    Booking();
    QDate startDate;
    QDate endDate;
    QString stringStart;
    QString stringEnd;
    QString program;
    bool monthly;
    int cost;
    QString lunch;
    QString room;
    QString clientId;
    int stayLength;
    QString wakeTime;
    double paidTotal;
};

#endif // BOOKING_H
