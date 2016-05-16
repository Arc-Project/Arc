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
    double cost;
    QString lunch;
    QString room;
    QString clientId;
    int stayLength;
    QString wakeTime;
    double paidTotal;
    QString bookID;
    QString roomId;
};

#endif // BOOKING_H
