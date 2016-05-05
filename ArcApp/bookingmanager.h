#ifndef BOOKINGMANAGER_H
#define BOOKINGMANAGER_H
#include <QDate>
#include <QMainWindow>
class bookingManager
{
public:
    bookingManager();
    bool checkValidDate(QDate start, QDate end);
private:

};

#endif // BOOKINGMANAGER_H
