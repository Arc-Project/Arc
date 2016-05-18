#ifndef MYCALENDAR_H
#define MYCALENDAR_H

#include <QDialog>
#include <QDate>
#include "client.h"
#include "databasemanager.h"
namespace Ui {
class MyCalendar;
}

class MyCalendar : public QDialog
{
    Q_OBJECT

public:
    explicit MyCalendar(QWidget *parent = 0);

    MyCalendar(QWidget *parent, QDate start, QDate end, Client * client, int mode, QString room);
    void populateCells();
    ~MyCalendar();
    void colorRow(int row, int column, QColor c);
    void addTime(int row, int col, QString time);
private slots:
    void on_calendarTable_cellClicked(int row, int column);
    void loadPrevious();
    void on_buttonBox_accepted();


private:
    QDate sDate, eDate;
    QDate calStart;
    int lowEnd, highEnd;
    int oldLunch[70];
    QTime wakeUps[70];
    QTime oldWakeUps[70];
    Ui::MyCalendar *ui;
    int mode;
    int wakeFlag[70];
    int oldWakeFlag[70];
    int lunchDays[70];
    int offset;
    int maxIns;
    QColor oneL, twoL, noL;
    Client * curClient;
    DatabaseManager * dbm;
    QString roomId;

};

#endif // MYCALENDAR_H
