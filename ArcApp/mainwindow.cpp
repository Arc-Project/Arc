#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
bool firstTime = true;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->stackedWidget);



    ui->makeBookingButton->hide();
    //mw = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bookButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(BOOKINGLOOKUP);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());
    if(firstTime){
        firstTime = false;
        getProgramCodes();
        bookingSetup();

    }
    ui->makeBookingButton->hide();
    ui->monthCheck->setChecked(false);
}

void MainWindow::on_clientButton_clicked()
{
     ui->stackedWidget->setCurrentIndex(CLIENTLOOKUP);
}

void MainWindow::on_paymentButton_clicked()
{
     ui->stackedWidget->setCurrentIndex(PAYMENTPAGE);
}

void MainWindow::on_editbookButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(EDITBOOKING);

}

void MainWindow::on_caseButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(CASEFILE);

}

void MainWindow::on_adminButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(ADMINPAGE);

}


void MainWindow::bookingSetup(){

    ui->bookingTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->bookingTable->verticalHeader()->hide();
    ui->bookingTable->horizontalHeader()->setStretchLastSection(true);
    ui->bookingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->bookingTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->bookingTable->setHorizontalHeaderLabels(QStringList() << "Room #" << "Location" << "Program" << "Description" << "Cost" << "Monthly");

}

void MainWindow::on_bookingSearchButton_clicked()
{
    if(!book.checkValidDate(ui->startDateEdit->date(), ui->endDateEdit->date())){
        //Pop up error or something
        return;
    }
    ui->bookingTable->setRowCount(0);
    ui->bookingTable->clear();
    ui->bookingTable->setHorizontalHeaderLabels(QStringList() << "Room #" << "Location" << "Program" << "Description" << "Cost" << "Monthly");
    QString program = ui->programDropdown->currentText();
    QSqlQuery result = dbManager->getCurrentBooking(ui->startDateEdit->date(), ui->endDateEdit->date(), program);
    int numCols = result.record().count();

    int x = 0;
    while (result.next()) {
        ui->bookingTable->insertRow(x);
        for (int i = 0; i < numCols; ++i)
        {
            ui->bookingTable->setItem(x,i, new QTableWidgetItem(result.value(i).toString()));


        }

        x++;

    }

    dbManager->printAll(result);
    ui->makeBookingButton->show();
}
void MainWindow::setBooking(int row){
    curBook->clientId = "1";
    curBook->startDate = ui->startDateEdit->date();
    curBook->endDate = ui->endDateEdit->date();
    curBook->stringStart = ui->startDateEdit->date().toString(Qt::ISODate);
    curBook->stringEnd = ui->endDateEdit->date().toString(Qt::ISODate);
    curBook->monthly = ui->monthCheck->isChecked();
    curBook->program = ui->bookingTable->item(row, 2)->text();
    curBook->room = ui->bookingTable->item(row,0)->text();
    if(ui->monthCheck->isChecked()){
        curBook->cost = ui->bookingTable->item(row, 5)->text().toInt();
    }
    else{
        curBook->cost = (ui->endDateEdit->date().toJulianDay() - ui->startDateEdit->date().toJulianDay()) * ui->bookingTable->item(row, 4)->text().toInt();
    }
    curBook->stayLength = ui->endDateEdit->date().toJulianDay() - ui->startDateEdit->date().toJulianDay();

}

void MainWindow::on_makeBookingButton_clicked()
{
    int row = ui->bookingTable->selectionModel()->currentIndex().row();
    if(row == - 1){
        return;
    }
    ui->stackedWidget->setCurrentIndex(BOOKINGPAGE);
    int rowNum = ui->bookingTable->columnCount();
    QStringList data;
    curBook = new Booking;
    setBooking(row);
    ui->stackedWidget->setCurrentIndex(BOOKINGPAGE);
    populateBooking();
    ui->makeBookingButton_2->setEnabled(true);

}
void MainWindow::populateBooking(){
    //room, location, program, description, cost, program, start, end, stayLength
    ui->startLabel->setText(curBook->stringStart);
    ui->endLabel->setText(curBook->stringEnd);
    ui->roomLabel->setText(curBook->room);
    ui->costInput->setText(QString::number(curBook->cost));
    ui->programLabel->setText(curBook->program);
    ui->lengthOfStayLabel->setText(QString::number(curBook->stayLength));
    ui->wakeupCheck->setChecked(false);
    ui->lunchCheck->setChecked(false);
    if(curBook->monthly){
        ui->monthLabel->setText("YES");
    }
    else{
        ui->monthLabel->setText("NO");
    }
}

void MainWindow::getProgramCodes(){
    QSqlQuery result = dbManager->getPrograms();
    int i = 0;
    while(result.next()){
        ui->programDropdown->addItem(result.value(i++).toString());
    }
}

void MainWindow::on_EditUserButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(EDITUSERS);
}

void MainWindow::on_EditProgramButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(EDITPROGRAM);

}

void MainWindow::on_actionMain_Menu_triggered()
{
    ui->stackedWidget->setCurrentIndex(MAINMENU);
}


void MainWindow::on_actionDB_Connection_triggered()
{
    QSqlQuery results= dbManager->selectAll("Test");
    dbManager->printAll(results);
}

void MainWindow::on_bookingTable_itemClicked(QTableWidgetItem *item)
{
    int sel = item->row();
    qDebug() << sel;
}

void MainWindow::on_makeBookingButton_2_clicked()
{
    ui->makeBookingButton_2->setEnabled(false);
    if(ui->lunchCheck->checkState()){
        curBook->lunch = "YES";
    }
    else{
        curBook->lunch = "NO";
    }
    if(ui->wakeupCheck->checkState()){
        curBook->wakeTime = ui->wakeTime->time().toString();
    }
    else{
       curBook->wakeTime = "NO";
    }
    QString month;
    if(curBook->monthly){
        month = "YES";
    }
    else{
        month = "NO";
    }
    int cost = ui->costInput->text().toInt();
    QDate today = QDate::currentDate();
    QString values;
    QString todayDate = today.toString(Qt::ISODate);
    values = "'" + today.toString(Qt::ISODate) + "','" + curBook->stringStart + "','" + curBook->room + "','" +
             curBook->clientId + "','" + curBook->program + "','" + QString::number(cost) + "','" + curBook->stringStart
             + "','" + curBook->stringEnd + "','" + curBook->lunch + "','" + curBook->wakeTime + "'," + "'YES'" + ",'" + month + "'";
    QDate next = curBook->startDate;
    //QDate::fromString(ui->startLabel->text(), "yyyy-MM-dd");
    dbManager->insertBookingTable(values);
    for(int i = 1; i < curBook->stayLength; i++){
        QDate n = next.addDays(i);
        values = "'" + today.toString(Qt::ISODate) + "','" + n.toString(Qt::ISODate) + "','" + curBook->room + "','" +
                 curBook->clientId + "','" + curBook->program + "','" + QString::number(cost) + "','" + curBook->stringStart
                 + "','" + curBook->stringEnd + "','" + curBook->lunch + "','" + curBook->wakeTime + "'," + "'NO'" + ",'" + month + "'";
        dbManager->insertBookingTable(values);
    }

 }


void MainWindow::on_monthCheck_stateChanged(int arg1)
{
    if(arg1)
    {
        QDate month = ui->startDateEdit->date();
        month = month.addMonths(1);
        ui->endDateEdit->setDate(month);
    }
}
