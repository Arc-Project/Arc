#include "mycalendar.h"
#include "ui_mycalendar.h"
#include <QDebug>
#include <QtCore/qmath.h>
MyCalendar::MyCalendar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyCalendar)
{
    ui->setupUi(this);
}
MyCalendar::MyCalendar(QWidget *parent,  QDate start, QDate end, Client *client) :
    QDialog(parent),
    ui(new Ui::MyCalendar)
{
    curClient = client;
    sDate = start;
    eDate = end;
    oneL = QColor(220,20,190);
    twoL = QColor(15,40,225);
    noL = QColor(255,255,255);
    ui->setupUi(this);
    populateCells();
    for(int i = 0; i < 70; i++){
        lunchDays[i] = 0;
    }
    loadPrevious();
    ui->calendarTable->setSelectionMode(QTableWidget::NoSelection);
    ui->calendarTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

}
MyCalendar::~MyCalendar()
{
    delete ui;
}
void MyCalendar::populateCells(){
    QDate firstDay;
    int startCell, endCell;
    int curDay;
    curDay = sDate.day();
    curDay--;
    firstDay = sDate.addDays(curDay * -1);
    calStart = firstDay;
    int startIndex = firstDay.dayOfWeek();
    startIndex %= 7;
    offset = startIndex;
    int totalDays = firstDay.daysInMonth();
    int lengthOfStay = eDate.toJulianDay() - sDate.toJulianDay();
    int totalShow = eDate.month() - sDate.month();
    QDate nextMonth = sDate.addMonths(1);
    if(totalShow > 1)
        totalShow = 1;
    if(totalShow){
        totalShow = sDate.daysInMonth() + nextMonth.daysInMonth();
    }
    else{
        totalShow = sDate.daysInMonth();
    }
    double dDevis((totalShow + startIndex) / 7.0);
    int numRows = qCeil(dDevis);
    for(int i = 0; i < numRows; i++)
        ui->calendarTable->insertRow(i);
    qDebug() << startIndex << " <" << totalDays;
    int x = 1;
    int q = 1;
    int startUnlock = sDate.day();
    int endUnlock = startUnlock + lengthOfStay - 1;
    lowEnd = startUnlock -1 + offset;
    highEnd = endUnlock - 1 + offset;
    ui->calendarTable->setHorizontalHeaderLabels(QStringList()<< "Sunday" << "Monday"  << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday");
    for(int i = startIndex; i < totalShow + startIndex; i++){

        ui->calendarTable->setItem(i / 7, i % 7, new QTableWidgetItem(QString::number(x)));
        if(q < startUnlock || q > endUnlock)
            ui->calendarTable->item(i / 7, i % 7)->setBackgroundColor(QColor(155,155,155));
        x++;
        q++;
        if(x > totalDays)
            x = 1;

    }
}

void MyCalendar::on_calendarTable_cellClicked(int row, int column)
{

    int clicked = row * 7 + column;
    if(clicked < lowEnd || clicked > highEnd)
        return;
    if(clicked > 62){
        return;
    }
    if(lunchDays[clicked] == 2)
        lunchDays[clicked] = 0;
    else{
        lunchDays[clicked]++;
    }
    switch(lunchDays[clicked]){
    case 0:
        colorRow(row, column, noL);
        break;
    case 1:
        colorRow(row, column, oneL);
        break;
    case 2:
        colorRow(row, column, twoL);
        break;

    }



    qDebug() << clicked;
}
void MyCalendar::colorRow(int row, int column, QColor c){
    ui->calendarTable->item(row,column)->setBackgroundColor(c);

}

void MyCalendar::on_buttonBox_accepted()
{
    QDate insDate;
    for(int i = 0; i < 70; i++){
        if(lunchDays[i]){
            insDate = calStart.addDays(i - offset);
            if(lunchDays[i] == oldLunch[i])
                continue;
            if(!oldLunch[i]){

                dbManager->setLunches(insDate, lunchDays[i], curClient->clientId);
                qDebug() << insDate;
            }
            else{
                insDate = calStart.addDays(i - offset);
                dbManager->updateLunches(insDate, lunchDays[i], curClient->clientId);
            }
        }
        else{
            if(oldLunch[i]){
                insDate = calStart.addDays(i - offset);
                dbManager->removeLunches(insDate, curClient->clientId);
            }
        }
    }
}
void MyCalendar::loadPrevious(){
    QSqlQuery result;
    int ins, row, col, nL;
    QDate pull;
    for(int i = 0; i < 70; i++){
        oldLunch[i] = 0;
    }
    result = dbManager->getLunches(sDate, eDate, curClient->clientId);
    while(result.next()){
        pull = QDate::fromString(result.value("LunchDate").toString(), "yyyy-MM-dd");
        ins = pull.toJulianDay() - calStart.toJulianDay();
        ins += offset;
        qDebug() << ins;
        nL = result.value("Number").toString().toInt();
        oldLunch[ins] = nL;
        lunchDays[ins] =  nL;
        row = ins / 7;
        col = ins % 7;
        oldLunch[ins] == 1 ? colorRow(row, col, oneL) : colorRow(row, col, twoL);

    }
}
