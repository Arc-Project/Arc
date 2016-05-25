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
MyCalendar::MyCalendar(QWidget *parent,  QDate start, QDate end, Client *client, int mode, QString room) :
    QDialog(parent),
    ui(new Ui::MyCalendar)
{
    this->mode = mode;
    curClient = client;
    sDate = start;
    eDate = end;
    roomId = room;
    oneL = QColor(255, 153, 204);
    twoL = QColor(102, 179, 255);
    noL = QColor(255,255,255);
    ui->setupUi(this);
    populateCells();

    if(mode == 1){
        ui->calendarTime->setHidden(true);
        ui->grpWakeup->setHidden(true);
    }
    if(mode == 2){
        ui->calOne->setHidden(true);
        ui->calTwo->setHidden(true);
        ui->grpLunch->setHidden(true);
    }
    for(int i = 0; i < 70; i++){
        lunchDays[i] = 0;
        wakeFlag[i] = 0;

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
        if(eDate.day() == 1){
            totalShow = sDate.daysInMonth();
        }
        else{
            totalShow = sDate.daysInMonth() + nextMonth.daysInMonth();
        }
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
    QDate insertString;
    int endUnlock = startUnlock + lengthOfStay - 1;
    lowEnd = startUnlock -1 + offset;
    highEnd = endUnlock - 1 + offset;
    ui->calendarTable->setHorizontalHeaderLabels(QStringList()<< "Sunday" << "Monday"  << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday");
    for(int i = startIndex; i < totalShow + startIndex; i++){
        insertString = calStart.addDays(q - 1);
        ui->calendarTable->setItem(i / 7, i % 7, new QTableWidgetItem(insertString.toString("MMM d")));
        if(q < startUnlock || q > endUnlock)
            ui->calendarTable->item(i / 7, i % 7)->setBackgroundColor(QColor(155,155,155));
        x++;
        q++;
        if(x > totalDays)
            x = 1;

    }
    maxIns = q - 1;
}

void MyCalendar::on_calendarTable_cellClicked(int row, int column)
{

    int clicked = row * 7 + column;
    if(clicked < lowEnd || clicked > highEnd)
        return;
    if(clicked > 62){
        return;
    }
    if(mode == 1){
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
    }
    else if(mode == 2){
        if(wakeFlag[clicked] == 1){
            QDate resetDate = calStart.addDays(clicked - offset);
            ui->calendarTable->item(row,column)->setText(resetDate.toString("MMM d"));

            wakeFlag[clicked] = 0;
        }
        else{
            ui->calendarTable->item(row,column)->setText(ui->calendarTable->item(row,column)->text() + "\n" + ui->calendarTime->time().toString("h:mm ap"));
            wakeUps[clicked] = ui->calendarTime->time();
             wakeFlag[clicked]++;
        }

    }


    qDebug() << clicked;
}
void MyCalendar::colorRow(int row, int column, QColor c){
    ui->calendarTable->item(row,column)->setBackgroundColor(c);

}

void MyCalendar::on_buttonBox_accepted()
{
    QDate insDate;
    if(mode == 1){
        for(int i = 0; i < 70; i++){
            if(lunchDays[i]){
                insDate = calStart.addDays(i - offset);
                if(lunchDays[i] == oldLunch[i])
                    continue;
                if(!oldLunch[i]){

                    dbManager->setLunches(insDate, lunchDays[i], curClient->clientId, roomId);
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
    else if(mode == 2){
        for(int i = 0; i < 70; i++){
            if(wakeFlag[i]){
                insDate = calStart.addDays(i - offset);
                if(oldWakeFlag[i]){
                    if(wakeUps[i] == oldWakeUps[i]){
                        continue;
                    }
                    else{
                        dbManager->updateWakeups(insDate, wakeUps[i].toString(), curClient->clientId);
                    }
                }else{
                    dbManager->setWakeup(insDate, wakeUps[i].toString(), curClient->clientId, roomId);
                }

            }
            else{
                if(oldWakeFlag[i]){
                    insDate = calStart.addDays(i - offset);

                    dbManager->deleteWakeups(insDate, curClient->clientId);
                }
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
        oldWakeFlag[i] = 0;
    }

    if(mode == 1){
        result = dbManager->getLunches(sDate, eDate, curClient->clientId);
        while(result.next()){
            pull = QDate::fromString(result.value("LunchDate").toString(), "yyyy-MM-dd");
            ins = pull.toJulianDay() - calStart.toJulianDay();
            ins += offset;
            if(ins > maxIns)
                continue;
            if(ins < 0)
                continue;
            qDebug() << ins;
            nL = result.value("Number").toString().toInt();
            oldLunch[ins] = nL;
            lunchDays[ins] =  nL;
            row = ins / 7;
            col = ins % 7;
            oldLunch[ins] == 1 ? colorRow(row, col, oneL) : colorRow(row, col, twoL);

        }
    }
    else if(mode == 2){
        result = dbManager->getWakeups(sDate, eDate, curClient->clientId);

        while(result.next()){
            pull = QDate::fromString(result.value("WakeDate").toString(), "yyyy-MM-dd");
            ins = pull.toJulianDay() - calStart.toJulianDay();
            ins += offset;
            if(ins > maxIns)
                continue;
            if(ins < 0)
                continue;
            qDebug() << ins;
            oldWakeUps[ins] = QTime::fromString(result.value("WakeTime").toString(), "hh:mm:ss");
            wakeUps[ins] = QTime::fromString(result.value("WakeTime").toString(), "hh:mm:ss");
            wakeFlag[ins] = 1;
            oldWakeFlag[ins] = 1;
            qDebug() << oldWakeUps[ins].toString();
            row = ins / 7;
            col = ins % 7;
            addTime(row, col, oldWakeUps[ins].toString("h:mm ap"));
        }
    }
}
void MyCalendar::addTime(int row, int col, QString time){
    ui->calendarTable->item(row,col)->setText(ui->calendarTable->item(row,col)->text() + " " + time);
}
