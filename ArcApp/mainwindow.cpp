#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "takephoto.h"
#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>
#include "payment.h"

//MyModel* checkoutModel;
Report *checkoutReport, *vacancyReport, *lunchReport, *wakeupReport;
bool firstTime = true;
std::vector<QTableWidget*> pcp_tables;
//QSqlQuery resultssss;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->stackedWidget);

    ui->makeBookingButton->hide();
    //mw = this;

    //default signal of stackedWidget
    //detect if the widget is changed
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(initCurrentWidget(int)));
    curClient = 0;
    curBook = 0;
    trans = 0;
    setPcpVector();

    checkoutReport = new Report(this, ui->checkout_tableView, CHECKOUT_REPORT);
//    vacancyReport = new Report(this, ui->vacancy_tableView, VACANCY_REPORT);
//    lunchReport = new Report(this, ui->lunch_tableView, LUNCH_REPORT);
//    wakeupReport = new Report(this, ui->wakeup_tableView, WAKEUP_REPORT);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*==============================================================================
DETECT WIDGET CHANGING SIGNAL
==============================================================================*/
//initialize widget when getinto that widget
void MainWindow::initCurrentWidget(int idx){
    switch(idx){
        case MAINMENU:  //WIDGET 0
            curClientID = "";
            break;
        case CLIENTLOOKUP:  //WIDGET 1
            if(ui->tableWidget_search_client->columnCount()>0){
            //init client search table
                ui->tableWidget_search_client->setColumnCount(0);
                ui->tableWidget_search_client->clear();
            }
            if(ui->tableWidget_clientInfo->columnCount()>0){
            //init client info table
                ui->tableWidget_clientInfo->setColumnCount(0);
                ui->tableWidget_clientInfo->clear();
                ui->tableWidget_clientInfo2->setColumnCount(0);
                ui->tableWidget_clientInfo2->clear();
            }
            //initimageview


            break;
        case BOOKINGLOOKUP: //WIDGET 2
            //initcode
            qDebug()<<"Client INFO";
            if(curClient != NULL){
                qDebug()<<"ID: " << curClientID << curClient->clientId;
                qDebug()<<"NAME: " << curClient->fullName;
                qDebug()<<"Balance: " << curClient->balance;
            }
            break;
        case BOOKINGPAGE: //WIDGET 3
            //initcode
            break;
        case PAYMENTPAGE: //WIDGET 4
            popManagePayment();
            break;
        case ADMINPAGE: //WIDGET 5
            //initcode
            break;
        case EDITUSERS: //WIDGET 6
            //initcode
            break;
        case EDITPROGRAM: //WIDGET 7
            //initcode
            break;
        case CASEFILE: //WIDGET 8
            //initcode
            break;
        case EDITBOOKING: //WIDGET 9
            //initcode
            break;
        case CLIENTREGISTER:    //WIDGET 10
            clear_client_register_form();
            defaultRegisterOptions();           //combobox item add
            if(curClientID != NULL)
                read_curClient_Information(curClientID);
            break;
        case 11:    //WIDGET 11
            //MainWindow::updateCheckoutView();
        default:
            qDebug()<<"NO information about stackWidget idx : "<<idx;

    }
}

void MainWindow::on_bookButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(CLIENTLOOKUP);
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate().addDays(1));
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
    ui->stackedWidget->setCurrentIndex(CLIENTLOOKUP);

}

void MainWindow::on_adminButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(ADMINPAGE);

}

/*==============================================================================
DEV TESTING BUTTONS (START)
==============================================================================*/
void MainWindow::on_actionDB_Connection_triggered()
{
    //QSqlQuery results= dbManager->selectAll("Test");
    //dbManager->printAll(results);
//    QStringList* data = new QStringList();
//    *data << "11" << "12" << "21" << "22";
//    checkoutModel->setData(data, 2, 2);
}

void MainWindow::on_actionTest_Query_triggered()
{
    ui->stackedWidget->setCurrentIndex(11);
    checkoutReport->updateModel(QDate::currentDate());
//    vacancyReport->updateModel(QDate::currentDate());
//    lunchReport->updateModel(QDate::currentDate());
//    wakeupReport->updateModel(QDate::currentDate());
}



void MainWindow::on_actionFile_Upload_triggered()
{
    QString strFilePath = MainWindow::browse();
    if (!strFilePath.isEmpty())
    {
        QtConcurrent::run(dbManager, &DatabaseManager::uploadThread, strFilePath);
    }
    else
    {
        qDebug() << "Empty file path";
    }
}

void MainWindow::on_actionDownload_Latest_Upload_triggered()
{
    QtConcurrent::run(dbManager, &DatabaseManager::downloadThread);
}

void MainWindow::on_actionPrint_Db_Connections_triggered()
{
    dbManager->printDbConnections();
}

void MainWindow::on_actionUpload_Display_Picture_triggered()
{
    QString strFilePath = MainWindow::browse();
    if (!strFilePath.isEmpty())
    {
        QtConcurrent::run(dbManager, &DatabaseManager::uploadProfilePicThread, strFilePath);
    }
    else
    {
        qDebug() << "Empty file path";
    }
}

void MainWindow::on_actionDownload_Profile_Picture_triggered()
{
    QImage* img = new QImage();
    img->scaledToWidth(300);
    dbManager->downloadProfilePic(img);

    MainWindow::addPic(*img);
}
/*==============================================================================
DEV TESTING BUTTONS (END)
==============================================================================*/
/*==============================================================================
DEV TESTING AUXILIARY FUNCTIONS (START)
==============================================================================*/
QString MainWindow::browse()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    QString strFilePath = dialog.getOpenFileName(this, tr("SelectFile"), "", tr("All Files (*.*)"));

    return strFilePath;
}
/*==============================================================================
DEV TESTING AUXILIARY FUNCTIONS (END)
==============================================================================*/
/*==============================================================================
REPORT FUNCTIONS (START)
==============================================================================*/
void MainWindow::updateCheckoutModel()
{
    QSqlDatabase tempDb = QSqlDatabase::database();
    QString connName = QString::number(dbManager->getDbCounter());
    if (dbManager->createDatabase(&tempDb, connName))
    {
        QSqlQuery query;
        if (dbManager->getCheckoutQuery(&query, QDate::currentDate()))
        {
//            int numCols = query.record().count();

//            int numRows = 0;
//            QStringList *data = new QStringList();
//            while (query.next()) {
//                numRows++;
//                for (int i = 0; i < numCols; ++i)
//                {
//                    *data << query.value(i).toString();
//                }
//            }
//            checkoutModel->setData(data, numRows, numCols);
            qDebug() << "test";
            //report->setData(&query);
        }
        tempDb.close();
        QSqlDatabase::removeDatabase(connName);
    }
}


void MainWindow::updateCheckoutView(QDate date)
{
    QSqlQuery query;    

    ui->checkout_tableWidget->setRowCount(0);
    if (dbManager->getCheckoutQuery(&query, date))
    {
        int rowNo = 0;
        while(query.next())
        {
            ui->checkout_tableWidget->insertRow(rowNo);
            for (int colNo = 0; colNo < NUM_CHKOUT_TBL_COLS; ++colNo)
            {
                if (colNo == 5 || colNo == 6)
                {
                    ui->checkout_tableWidget->setItem(rowNo, colNo,
                        new QTableWidgetItem("tbd"));
                }
                else if (colNo == 7)
                {
                    ui->checkout_tableWidget->setItem(rowNo, colNo,
                        new QTableWidgetItem(query.value(5).toString()));
                }
                else
                {
                    ui->checkout_tableWidget->setItem(rowNo, colNo,
                        new QTableWidgetItem(query.value(colNo).toString()));
                }
            }
            rowNo++;
        }
    }
    else
    {
        qDebug() << "updateCheckoutView() failed";
    }
    ui->checkout_tableWidget->show();
}

void MainWindow::updateVacancyView(QDate date)
{
    QSqlQuery query;
    ui->vacancy_tableWidget->setRowCount(0);
    if (dbManager->getVacancyQuery(&query, date))
    {
        int rowNo = 0;
        while(query.next())
        {
            ui->vacancy_tableWidget->insertRow(rowNo);
            for (int colNo = 0; colNo < NUM_VACANCY_TBL_COLS; ++colNo)
            {
                ui->vacancy_tableWidget->setItem(rowNo,
                    colNo, new QTableWidgetItem(query.value(colNo).toString()));
            }
            rowNo++;
        }
    }
    else
    {
        qDebug() << "updateVacancyView() failed";
    }
    ui->vacancy_tableWidget->show();
}

void MainWindow::updateLunchView(QDate date)
{
    QSqlQuery query;
    ui->lunch_tableWidget->setRowCount(0);
    if (dbManager->getLunchQuery(&query, date))
    {
        int rowNo = 0;
        while(query.next())
        {
            ui->lunch_tableWidget->insertRow(rowNo);
            for (int colNo = 0; colNo < NUM_LUNCH_TBL_COLS; ++colNo)
            {
                ui->lunch_tableWidget->setItem(rowNo,
                    colNo, new QTableWidgetItem(query.value(colNo).toString()));
            }
            rowNo++;
        }
    }
    else
    {
        qDebug() << "updateLunchView() failed";
    }
    ui->lunch_tableWidget->show();
}

void MainWindow::updateWakeupView(QDate date)
{
    QSqlQuery query;
    ui->wakeup_tableWidget->setRowCount(0);
    if (dbManager->getWakeupQuery(&query, date))
    {
        int rowNo = 0;
        while(query.next())
        {
            ui->wakeup_tableWidget->insertRow(rowNo);
            for (int colNo = 0; colNo < NUM_WAKEUP_TBL_COLS; ++colNo)
            {
                ui->wakeup_tableWidget->setItem(rowNo,
                    colNo, new QTableWidgetItem(query.value(colNo).toString()));
            }
            rowNo++;
        }
    }
    else
    {
        qDebug() << "updateWakeupView() failed";
    }
    ui->wakeup_tableWidget->show();
}
/*==============================================================================
REPORT FUNCTIONS (END)
==============================================================================*/

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
            if(i == 4){
                ui->bookingTable->setItem(x,i, new QTableWidgetItem(QString::number(result.value(i).toString().toDouble(), 'f', 2)));
                continue;
            }
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
    curBook->stayLength = ui->endDateEdit->date().toJulianDay() - ui->startDateEdit->date().toJulianDay();
    double potentialCost = 999999;
    double dailyCost = 0;
    QString dayCost = QString::number(ui->bookingTable->item(row, 4)->text().toDouble(), 'f', 2);
    dailyCost = dayCost.toDouble();
    dailyCost = curBook->stayLength * dailyCost;
    if(ui->monthCheck->isChecked()){

        potentialCost = ui->bookingTable->item(row, 5)->text().toInt();
        if(dailyCost < potentialCost){
            curBook->cost = dailyCost;
        }
        else{
            curBook->cost = potentialCost;
        }
    }
    else{
        curBook->cost = dailyCost;
    }

}

void MainWindow::on_makeBookingButton_clicked()
{
    int row = ui->bookingTable->selectionModel()->currentIndex().row();
    if(row == - 1){
        return;
    }
    //curClient = new Client();
    popClientFromId("1");
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
    // - curBook->cost + curBook->paidTotal, 'f', 2)
    ui->stayLabel->setText(QString::number(curClient->balance));
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
        ui->programDropdown->addItem(result.value(0).toString());
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
    double cost = QString::number(ui->costInput->text().toDouble(), 'f', 2).toDouble();
    QDate today = QDate::currentDate();
    QString values;
    QString todayDate = today.toString(Qt::ISODate);
    values = "'" + today.toString(Qt::ISODate) + "','" + curBook->stringStart + "','" + curBook->room + "','" +
             curBook->clientId + "','" + curBook->program + "','" + QString::number(cost) + "','" + curBook->stringStart
             + "','" + curBook->stringEnd + "','" + curBook->lunch + "','" + curBook->wakeTime + "'," + "'YES'" + ",'" + month + "','" + "Eunwon'";
    QDate next = curBook->startDate;
    //QDate::fromString(ui->startLabel->text(), "yyyy-MM-dd");
    curBook->cost = cost;
    if(!dbManager->insertBookingTable(values)){
        qDebug() << "ERROR INSERTING BOOKING";
    }
    if(!dbManager->updateBalance(curClient->balance - curBook->cost, curClient->clientId)){
        qDebug() << "ERROR ADDING TO BALANCE UPDATE";
    }
    /*for(int i = 1; i < curBook->stayLength; i++){
        QDate n = next.addDays(i);
        values = "'" + today.toString(Qt::ISODate) + "','" + n.toString(Qt::ISODate) + "','" + curBook->room + "','" +
                 curBook->clientId + "','" + curBook->program + "','" + QString::number(cost) + "','" + curBook->stringStart
                 + "','" + curBook->stringEnd + "','" + curBook->lunch + "','" + curBook->wakeTime + "'," + "'NO'" + ",'" + month + "','" + "Eunwon'";
        if(!dbManager->insertBookingTable(values)){
            qDebug() << "ERROR INSERTING BOOKING";
        }
    }*/

    ui->stackedWidget->setCurrentIndex(CONFIRMBOOKING);
    populateConfirm();

 }

void MainWindow::populateConfirm(){
    ui->confirmCost->setText(QString::number(curBook->cost, 'f', 2));
    ui->confirmEnd->setText(curBook->stringEnd);
    ui->confirmStart->setText(curBook->stringStart);
    ui->confirmLength->setText(QString::number(curBook->stayLength));
    if(curBook->monthly){
        ui->confirmMonthly->setText("YES");
    }else{
        ui->confirmMonthly->setText("NO");
    }
    ui->confirmPaid->setText(QString::number(curClient->balance));
    ui->confirmProgram->setText(curBook->program);
    ui->confirmWakeup->setText(curBook->wakeTime);
    ui->confirmLunch->setText(curBook->lunch);

}

void MainWindow::on_monthCheck_stateChanged(int arg1)
{
    if(arg1)
    {
        QDate month = ui->startDateEdit->date();
        //month = month.addMonths(1);
        int days = month.daysInMonth();
        days = days - month.day();
        month = month.addDays(days);
        ui->endDateEdit->setDate(month);
    }
}

void MainWindow::on_pushButton_RegisterClient_clicked()
{

    ui->stackedWidget->setCurrentIndex(10);
    ui->label_cl_infoedit_title->setText("Register Client");
    ui->button_register_client->setText("Register");
    ui->dateEdit_cl_rulesign->setDate(QDate::currentDate());
    defaultRegisterOptions();
}

void MainWindow::on_pushButton_editClientInfo_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->label_cl_infoedit_title->setText("Edit Client Information");
    ui->button_register_client->setText("Edit");
}

void MainWindow::on_button_cancel_client_register_clicked()
{
    clear_client_register_form();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_reportsButton_clicked()
{
//    MainWindow::updateCheckoutView();
//    MainWindow::updateVacancyView();
//    MainWindow::updateLunchView();
//    MainWindow::updateWakeupView();
    ui->stackedWidget->setCurrentIndex(11);
}

/*===================================================================
  REGISTRATION PAGE
  ===================================================================*/

//Client Regiter widget [TAKE A PICTURE] button
void MainWindow::on_button_cl_takePic_clicked()
{
    TakePhoto *camDialog = new TakePhoto();

    connect(camDialog, SIGNAL(showPic(QImage)), this, SLOT(addPic(QImage)));
    camDialog->show();
}
/*------------------------------------------------------------------
  add picture into graphicview (after taking picture in pic dialog
  ------------------------------------------------------------------*/
void MainWindow::addPic(QImage pict){

  //  qDebug()<<"ADDPIC";
    profilePic = pict.copy();
    QPixmap item = QPixmap::fromImage(pict);
    QPixmap scaled = QPixmap(item.scaledToWidth((int)(ui->graphicsView_cl_pic->width()*0.9), Qt::SmoothTransformation));
    QGraphicsScene *scene = new QGraphicsScene();
    scene->addPixmap(QPixmap(scaled));
    ui->graphicsView_cl_pic->setScene(scene);
    ui->graphicsView_cl_pic->show();
}

void MainWindow::on_button_cl_delPic_clicked()
{
    QGraphicsScene *scene = new QGraphicsScene();
    scene->clear();
    profilePic = (QImage)NULL;
    ui->graphicsView_cl_pic->setScene(scene);

    //delete picture function to database

}

void MainWindow::on_button_clear_client_regForm_clicked()
{
    clear_client_register_form();
}

void MainWindow::getListRegisterFields(QStringList* fieldList)
{
    *fieldList << ui->lineEdit_cl_fName->text()
               << ui->lineEdit_cl_mName->text()
               << ui->lineEdit_cl_lName->text()
               << ui->dateEdit_cl_dob->date().toString("yyyy-MM-dd")
               << ui->lineEdit_cl_SIN->text()
               << ui->lineEdit_cl_GANum->text()
               << 0//ui->comboBox_cl_caseWorker->currentText() //grab value from case worker dropdown I don't know how to do it
               << QString::number(ui->checkBox_cl_comm->isChecked())
               << QString::number(ui->checkBox_cl_parolee->isChecked())
               << ui->dateEdit_cl_rulesign->date().toString("yyyy-MM-dd")
               << ui->lineEdit_cl_nok_name->text()
               << ui->lineEdit_cl_nok_relationship->text()
               << ui->lineEdit_cl_nok_loc->text()
               << ui->lineEdit_cl_nok_ContactNo->text()
               << ui->lineEdit_cl_phys_name->text()
               << ui->lineEdit_cl_phys_ContactNo->text()
               << ui->lineEdit_cl_Msd_Name->text()
               << ui->lineEdit_cl_Msd_ContactNo->text()
               << ui->comboBox_cl_status->currentText() //grab value from status dropdown
               << ui->plainTextEdit_cl_comments->toPlainText();

}

void MainWindow::clear_client_register_form(){
    ui->lineEdit_cl_fName->clear();
    ui->lineEdit_cl_mName->clear();
    ui->lineEdit_cl_lName->clear();
    ui->lineEdit_cl_SIN->clear();
    ui->lineEdit_cl_GANum->clear();
    ui->comboBox_cl_caseWorker->setCurrentIndex(0);
    ui->lineEdit_cl_nok_name->clear();
    ui->lineEdit_cl_nok_relationship->clear();
    ui->lineEdit_cl_nok_loc->clear();
    ui->lineEdit_cl_nok_ContactNo->clear();
    ui->lineEdit_cl_phys_name->clear();
    ui->lineEdit_cl_phys_ContactNo->clear();
    ui->lineEdit_cl_Msd_Name->clear();
    ui->lineEdit_cl_Msd_ContactNo->clear();
    ui->comboBox_cl_status->setCurrentIndex(0);
    ui->plainTextEdit_cl_comments->clear();
    QDate defaultDob= QDate::fromString("1990-01-01","yyyy-MM-dd");
    ui->dateEdit_cl_dob->setDate(defaultDob);
    ui->dateEdit_cl_rulesign->setDate(QDate::currentDate());
    on_button_cl_delPic_clicked();
}

//read client information to edit
void MainWindow::read_curClient_Information(QString ClientId){
    QString searchClientQ = "SELECT * FROM Client WHERE ClientId = "+ ClientId;
    qDebug()<<"SEARCH QUERY: " + searchClientQ;
    QSqlQuery clientInfo = dbManager->execQuery("SELECT * FROM Client WHERE ClientId = "+ ClientId);
    clientInfo.next();
    //dbManager->printAll(clientInfo);
    //input currentValue;

    qDebug()<<"FNAme: "<<clientInfo.value(1).toString()<<"MNAme: "<<clientInfo.value(2).toString()<<"LNAME: "<<clientInfo.value(3).toString();
    qDebug()<<"DOB: "<<clientInfo.value(4).toString() <<"GANUM: "<<clientInfo.value(6).toString()<<"SIN: "<<clientInfo.value(7).toString();

    ui->lineEdit_cl_fName->setText(clientInfo.value(1).toString());

    ui->lineEdit_cl_mName->setText(clientInfo.value(2).toString());
    ui->lineEdit_cl_lName->setText(clientInfo.value(3).toString());
    ui->dateEdit_cl_dob->setDate(QDate::fromString(clientInfo.value(4).toString(),"yyyy-MM-dd"));
    //balnace?
    ui->comboBox_cl_caseWorker->setCurrentText(clientInfo.value(23).toString());
    ui->lineEdit_cl_SIN->setText(clientInfo.value(6).toString());
    ui->lineEdit_cl_GANum->setText(clientInfo.value(7).toString());
    ui->checkBox_cl_parolee->setChecked(clientInfo.value(8).toBool());
    ui->checkBox_cl_comm->setChecked(clientInfo.value(9).toBool());
    ui->dateEdit_cl_rulesign->setDate(QDate::fromString(clientInfo.value(10).toString(),"yyyy-MM-dd"));

    //NEXT OF KIN FIELD
    ui->lineEdit_cl_nok_name->setText(clientInfo.value(11).toString());
    ui->lineEdit_cl_nok_relationship->setText(clientInfo.value(12).toString());
    ui->lineEdit_cl_nok_loc->setText(clientInfo.value(13).toString());
    ui->lineEdit_cl_nok_ContactNo->setText(clientInfo.value(14).toString());

    //Physician
    ui->lineEdit_cl_phys_name->setText(clientInfo.value(15).toString());
    ui->lineEdit_cl_phys_ContactNo->setText(clientInfo.value(16).toString());

    //WSDWorker
    ui->comboBox_cl_status->setCurrentText(clientInfo.value(19).toString());
}

//Client information input and register click
void MainWindow::on_button_register_client_clicked()
{
    QString queryOption;
    if(ui->button_register_client->text()=="Register"){
        queryOption = "SELECT";
    }
    else
        queryOption = "ALTER";

    if (MainWindow::check_client_register_form())
    {
        if(ui->label_cl_infoedit_title->text() == "Register Client")
        {
            QStringList registerFieldList;
            MainWindow::getListRegisterFields(&registerFieldList);
            if (dbManager->insertClientWithPic(&registerFieldList, &profilePic))
            {
                qDebug() << "Client registered successfully";
            }
            else
            {
                qDebug() << "Could not register client";
            }
        }
        else
        {
            qDebug() << "Edit Client";
        }
        clear_client_register_form();
        ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
        qDebug() << "Register form check was false";
    }
}


//check if the value is valid or not
bool MainWindow::check_client_register_form(){
    if(ui->lineEdit_cl_fName->text().isEmpty()
            && ui->lineEdit_cl_mName->text().isEmpty()
            && ui->lineEdit_cl_lName->text().isEmpty()){
        ui->lineEdit_cl_fName->setText("anonymous");
    }

    return true;
}

void MainWindow::defaultRegisterOptions(){
    //add caseWorker Name
    QString caseWorkerquery = "SELECT Username FROM Employee WHERE Role = 'CASE WORKER' ORDER BY Username";
    QSqlQuery caseWorkers = dbManager->execQuery(caseWorkerquery);
    //dbManager->printAll(caseWorkers);
    if(ui->comboBox_cl_caseWorker->findText("NONE")==-1){
        ui->comboBox_cl_caseWorker->addItem("NONE");

    }
    while(caseWorkers.next()){
        qDebug()<<"CASEWORKER: " <<caseWorkers.value(0).toString();
        if(ui->comboBox_cl_caseWorker->findText(caseWorkers.value(0).toString())==-1)
            ui->comboBox_cl_caseWorker->addItem(caseWorkers.value(0).toString());
    }
    if(ui->comboBox_cl_status->findText("Green")==-1){
        ui->comboBox_cl_status->addItem("Green");
        ui->comboBox_cl_status->addItem("Yellow");
        ui->comboBox_cl_status->addItem("Red");
    }

}


/*==============================================================================
SEARCH CLIENTS USING NAME
==============================================================================*/
//search client
void MainWindow::on_pushButton_search_client_clicked()
{
    qDebug() <<"START SEARCH CLIENT";
    QString clientName = ui->lineEdit_search_clientName->text();
    QString searchQuery = "SELECT ClientId, FirstName, LastName, Dob, Balance FROM Client WHERE LastName LIKE '%"+clientName+"%' OR FirstName Like '%"+clientName+"%'";
    QSqlQuery results = dbManager->execQuery(searchQuery);
    setup_searchClientTable(results);

    QSqlQuery resultQ;
    /*
    if(!(dbManager->searchClientList(&resultQ, curClientID)))
    {
        qDebug()<<"Select Fail";
        return;
    }
*/
    connect(ui->tableWidget_search_client, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(selected_client_info(int,int)));
    // dbManager->printAll(results);

}


void MainWindow::setup_searchClientTable(QSqlQuery results){

    ui->tableWidget_search_client->setRowCount(0);

    int colCnt = results.record().count();
    ui->tableWidget_search_client->setColumnCount(colCnt);
    ui->tableWidget_search_client->clear();

    ui->tableWidget_search_client->setHorizontalHeaderLabels(QStringList()<<"ClientID"<<"FirstName"<<"LastName"<<"DateOfBirth"<<"Balance");
    int row =0;
    while(results.next()){
        ui->tableWidget_search_client->insertRow(row);
        for(int i =0; i<colCnt; i++){
            ui->tableWidget_search_client->setItem(row, i, new QTableWidgetItem(results.value(i).toString()));
            //qDebug() <<"row : "<<row << ", col: " << i << "item" << results.value(i).toString();
        }
        row++;
    }
    ui->tableWidget_search_client->show();

}




//get client information after searching

void MainWindow::selected_client_info(int nRow, int nCol)
{
    if(!pic_available || !table_available)
        return;

    curClientID = ui->tableWidget_search_client->item(nRow, 0)->text();
/*    QSqlQuery resultQ;

    if(!(dbManager->searchClientList(&resultQ, curClientID)))
    {
        qDebug()<<"Select Fail";
        return;
    }

*/
//    dbManager->printAll(resultQ);





//    qDebug()<<"GET DATA:" << curClientID;

/*
    pic_available = false;
    QtConcurrent::run(this, &displayPicThread, val);
    */
    table_available = false;
    QtConcurrent::run(this, &displayClientInfoThread, curClientID);

//    qDebug()<<"Finish Select Query to tableview";


}

void MainWindow::clientSearchedInfo(){

    QGraphicsScene *scene = new QGraphicsScene();
    scene->clear();
    ui->graphicsView_getInfo->setScene(scene);
}

void MainWindow::displayClientInfoThread(QString val){

    qDebug()<<"DISPLAY THREAD: " <<val;

    QString searchQuery = "SELECT FirstName, MiddleName, LastName, Dob, Balance, SinNo, GaNo, IsParolee, AllowComm, DateRulesSigned, status FROM Client WHERE ClientId =" + val;
   // QString searchQuery = "SELECT FirstName, MiddleName, LastName, Dob, Balance FROM Client WHERE ClientId =" + val;
    QSqlQuery clientInfoR = dbManager->execQuery(searchQuery);

    ui->tableWidget_clientInfo->setRowCount(0);
    ui->tableWidget_clientInfo2->setRowCount(0);

    int column = clientInfoR.record().count();
    ui->tableWidget_clientInfo->setColumnCount(5);
    ui->tableWidget_clientInfo->clear();
//    ui->tableWidget_clientInfo->setHorizontalHeaderLabels(QStringList()<<"FirstName"<< "MiddleName"<< "LastName" << "Dob" << "Balance"<< "SinNo" << "GaNo" << "IsParolee" << "AllowComm" << "DateRulesSigned");

    ui->tableWidget_clientInfo->setHorizontalHeaderLabels(QStringList()<<"FirstName"<< "MiddleName"<< "LastName" << "Dob" << "Balance");
/*
    ui->tableWidget_clientInfo2->setColumnCount(5);
    ui->tableWidget_clientInfo2->clear();
    ui->tableWidget_clientInfo2->setHorizontalHeaderLabels(QStringList()<<"SinNo" << "GaNo" << "IsParolee" << "AllowComm" << "DateRulesSigned");
*/
    QSqlQuery clientInfo = clientInfoR;
    int row = 0;
    int col = 0;
/*
    while(clientInfoR.next()){
        ui->tableWidget_clientInfo->insertRow(row);
 //       ui->tableWidget_clientInfo2->insertRow(row);
        for(col =0; col <5; col++){
            ui->tableWidget_clientInfo->setItem(row, col, new QTableWidgetItem(clientInfoR.value(col).toString()));
        //    qDebug() <<"row : "<<row << ", col: " << col << "item" << clientInfoR.value(col).toString();
        }

        while(col<column){
            ui->tableWidget_clientInfo2->setItem(row, col-5, new QTableWidgetItem(clientInfoR.value(col).toString()));
         //   qDebug() <<"row : "<<row << ", col: " << col << "item" << clientInfoR.value(col).toString();
            col++;
        }

        row++;
    }

   ui->tableWidget_clientInfo->show();
*/
//   ui->tableWidget_clientInfo2->show();

   clientInfo.next();
   ui->lineEdit_cl_info_fName->setText(clientInfo.value(0).toString());
   ui->lineEdit_cl_info_mName->setText(clientInfo.value(1).toString());
   ui->lineEdit_cl_info_lName->setText(clientInfo.value(2).toString());
   ui->lineEdit_cl_info_dob->setText(clientInfo.value(3).toString());
   ui->label_cl_info_balance_amt->setText(clientInfo.value(4).toString());
   ui->lineEdit_cl_info_SIN->setText(clientInfo.value(5).toString());
   ui->lineEdit_cl_info_gaNum->setText(clientInfo.value(6).toString());
   ui->lineEdit_cl_info_payrolee->setText(clientInfo.value(7).toBool()?"YES":"NO");
   ui->lineEdit_cl_info_allowComm->setText(clientInfo.value(8).toBool()?"Yes":"NO");
   ui->lineEdit_cl_info_ruleSignDate->setText(clientInfo.value(9).toString());
   ui->label_cl_info_status->setText(clientInfo.value(10).toString());

   table_available = true;


}

void MainWindow::displayPicThread(QString val)
{
    qDebug()<<"displayPicThread";
    QImage *ClientFace = new QImage();
    if(dbManager->downloadProfilePic2(ClientFace, val)){
    QPixmap item2 = QPixmap::fromImage(*ClientFace);
    QPixmap scaled = QPixmap(item2.scaledToWidth((int)(ui->graphicsView_getInfo->width()*0.9), Qt::SmoothTransformation));
    QGraphicsScene *scene2 = new QGraphicsScene();
    scene2->addPixmap(QPixmap(scaled));
    ui->graphicsView_getInfo->setScene(scene2);
    ui->graphicsView_getInfo->show();
    pic_available=true;
    }
}

void MainWindow::on_paymentButton_2_clicked()
{
    trans = new transaction();
    double owed;
    //owed = curBook->cost;
    owed = ui->costInput->text().toDouble();
    payment * pay = new payment(this, trans, curClient->balance, owed , curClient, curBook, true);
    pay->exec();
    ui->stayLabel->setText(QString::number(curClient->balance, 'f', 2));
    qDebug() << "Done";


}

// the add user button
void MainWindow::on_btn_createNewUser_clicked()
{
    // temporary disable stuff
    // obtain username and pw and role from UI
    QString uname = ui->le_userName->text();
    QString pw = ui->le_password->text();

    if (uname.length() == 0) {
        ui->lbl_editUserWarning->setText("Enter a Username");
        return;
    }

    if (pw.length() == 0) {
        ui->lbl_editUserWarning->setText("Enter a Password");
        return;
    }

    // first, check to see if the username is taken
    QSqlQuery queryResults = dbManager->findUser(uname);
    int numrows = queryResults.numRowsAffected();

    if (numrows > 0) {
        ui->lbl_editUserWarning->setText("This username is already taken");
        return;
    } else {
        QSqlQuery queryResults = dbManager->addNewEmployee(uname, pw, ui->comboBox->currentText());
        int numrows = queryResults.numRowsAffected();

        if (numrows != 0) {
            ui->lbl_editUserWarning->setText("Employee added");
            QStandardItemModel * model = new QStandardItemModel(0,0);
            model->clear();
            ui->tableWidget_3->clear();
            ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidget_3->setColumnCount(3);
            ui->tableWidget_3->setRowCount(0);
            ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");

            ui->comboBox->setCurrentIndex(0);
            ui->le_userName->setText("");
            ui->le_password->setText("");
            ui->le_users->setText("");
        } else {
            ui->lbl_editUserWarning->setText("Something went wrong - please try again");
        }
    }
}

void MainWindow::on_editSearch_clicked()
{
    ui->editLookupTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->editLookupTable->verticalHeader()->hide();
    ui->editLookupTable->horizontalHeader()->setStretchLastSection(true);
    ui->editLookupTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->editLookupTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->editLookupTable->setRowCount(0);
    ui->editLookupTable->clear();
    ui->editLookupTable->setHorizontalHeaderLabels(QStringList()
                                                << "Created" << "Start" << "End" << "Monthly" << "Room" << "Client" << "Program" << "Cost"
                                                 << "Lunch" << "Wakeup" << "" << "");
    ui->editLookupTable->setColumnHidden(10, true);
    ui->editLookupTable->setColumnHidden(11, true);

    QSqlQuery result;
    QString user = "";
    if(ui->editClient->text() != ""){
        user = ui->editClient->text();
    }
    result = dbManager->getActiveBooking(user, true);
    int numCols = result.record().count();
    //dbManager->printAll(result);
    int x = 0;
    int qt = result.size();
    qDebug() << qt;
    while (result.next()) {
        ui->editLookupTable->insertRow(x);


        QStringList row;
        row << result.value(1).toString() << result.value(7).toString() << result.value(8).toString() << result.value(12).toString()
            << result.value(3).toString() << result.value(13).toString() << result.value(5).toString() << result.value(6).toString()
                  << result.value(9).toString() << result.value(10).toString() << result.value(4).toString() << result.value(0).toString();
        for (int i = 0; i < 12; ++i)
        {
            ui->editLookupTable->setItem(x,i, new QTableWidgetItem(row.at(i)));


        }
        x++;


    }
}
void MainWindow::on_btn_dailyReport_clicked()
{
    ui->swdg_reports->setCurrentIndex(DAILYREPORT);
}

void MainWindow::on_btn_shiftReport_clicked()
{
    ui->swdg_reports->setCurrentIndex(SHIFTREPORT);
}

void MainWindow::on_btn_dailyLog_clicked()
{
    ui->swdg_reports->setCurrentIndex(DAILYLOG);
}

void MainWindow::on_btn_floatCount_clicked()
{
    ui->swdg_reports->setCurrentIndex(FLOATCOUNT);
}

void MainWindow::on_confirmationFinal_clicked()
{
    curBook = 0;
    curClient = 0;
    trans = 0;
    ui->stackedWidget->setCurrentIndex(MAINMENU);
}


void MainWindow::on_editButton_clicked()
{
    int row = ui->editLookupTable->selectionModel()->currentIndex().row();
    if(row == - 1){
        return;
    }
    curBook = new Booking();
    popBookFromRow();
    popClientFromId(ui->editLookupTable->item(row, 10)->text());
    ui->stackedWidget->setCurrentIndex(EDITPAGE);
    popEditPage();
}
void MainWindow::popClientFromId(QString id){
    QSqlQuery result;
    curClient = new Client();

    result = dbManager->pullClient(id);
    result.seek(0, false);
    curClient->clientId = id;
    curClient->fName = result.record().value("FirstName").toString();
    curClient->mName = result.record().value("MiddleName").toString();
    curClient->lName = result.record().value("LastName").toString();
    curClient->fullName = curClient->fName + " " +  curClient->mName + " "
            + curClient->lName;
    curClient->balance = result.record().value("Balance").toString().toDouble();



}

void MainWindow::popEditPage(){

    QSqlQuery result;
    result = dbManager->getPrograms();
    QString curProgram;
    QString compProgram;
    int index = 0;
    int t = 0;
    ui->editOC->setText("Original Cost: " + QString::number(curBook->cost));
    curProgram = curBook->program;
    while(result.next()){
        compProgram = result.value(0).toString();
        if(compProgram == curProgram)
            index = t;
        ui->editProgram_2->addItem(compProgram);
        t++;

    }
    ui->editRoomLabel->setText(curBook->room);
    ui->editProgram_2->setCurrentIndex(index);
    ui->editDate->setDate(curBook->endDate);
    ui->editCost->setText(QString::number(curBook->cost));
    curBook->lunch == "YES" ? ui->editLunch->setChecked(true) : ui->editLunch->setChecked(false);
    if(curBook->wakeTime != "NO"){
        ui->editWakeup->setTime(QTime::fromString(curBook->wakeTime));
        ui->editWakeCheck->setChecked(true);
    }


}

void MainWindow::popBookFromRow(){
    int row = ui->editLookupTable->selectionModel()->currentIndex().row();
    if(row == - 1){
        return;
    }
    curBook->cost = ui->editLookupTable->item(row,7)->text().toDouble();
    curBook->stringStart = ui->editLookupTable->item(row, 1)->text();
    curBook->startDate = QDate::fromString(curBook->stringStart, "yyyy-MM-dd");
    curBook->stringEnd = ui->editLookupTable->item(row, 2)->text();
    curBook->endDate = QDate::fromString(curBook->stringEnd, "yyyy-MM-dd");
    curBook->stayLength = curBook->endDate.toJulianDay() - curBook->startDate.toJulianDay();
    curBook->lunch = ui->editLookupTable->item(row,8)->text();
    if(ui->editLookupTable->item(row,3)->text() == "YES"){
      curBook->monthly = true;
         }
     else{
         curBook->monthly = false;
     }
    curBook->clientId = ui->editLookupTable->item(row, 10)->text();
    curBook->program = ui->editLookupTable->item(row,6)->text();
    curBook->room = ui->editLookupTable->item(row,4)->text();
    curBook->wakeTime = ui->editLookupTable->item(row,9)->text();
    curBook->cost = ui->editLookupTable->item(row, 7)->text().toDouble();
    curBook->bookID = ui->editLookupTable->item(row, 11)->text();

}

void MainWindow::on_btn_listAllUsers_clicked()
{
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->clear();
    ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);

    QSqlQuery result = dbManager->execQuery("SELECT Username, Password, Role FROM Employee");

    int numCols = result.record().count();
    ui->tableWidget_3->setColumnCount(numCols);
    ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");
    int x = 0;
    int qt = result.size();
    qDebug() << qt;
    while (result.next()) {
        ui->tableWidget_3->insertRow(x);
        QStringList row;
        row << result.value(0).toString() << result.value(1).toString() << result.value(2).toString();
        for (int i = 0; i < 3; ++i)
        {
            ui->tableWidget_3->setItem(x, i, new QTableWidgetItem(row.at(i)));
        }
        x++;
    }
}

void MainWindow::on_btn_searchUsers_clicked()
{
    QString ename = ui->le_users->text();
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->clear();
    ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);

    QSqlQuery result = dbManager->execQuery("SELECT Username, Password, Role FROM Employee WHERE Username LIKE '%"+ ename +"%'");

    int numCols = result.record().count();
    ui->tableWidget_3->setColumnCount(numCols);
    ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");
    int x = 0;
    int qt = result.size();
    qDebug() << qt;
    while (result.next()) {
        ui->tableWidget_3->insertRow(x);
        QStringList row;
        row << result.value(0).toString() << result.value(1).toString() << result.value(2).toString();
        for (int i = 0; i < 3; ++i)
        {
            ui->tableWidget_3->setItem(x, i, new QTableWidgetItem(row.at(i)));
        }
        x++;
    }


//    QSqlQuery results = dbManager->execQuery("SELECT Username, Password, Role FROM Employee WHERE Username LIKE '%"+ ename +"%'");
//    QSqlQueryModel *model = new QSqlQueryModel();
//    model->setQuery(results);

////    ui->tableWidget_3->setModel(model);
////    ui->tableWidget_3->horizontalHeader()->model()->setHeaderData(0, Qt::Horizontal, "Username");
////    ui->tableWidget_3->horizontalHeader()->model()->setHeaderData(1, Qt::Horizontal, "Password");
////    ui->tableWidget_3->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, "Role");
}

// double clicked employee
void MainWindow::on_tableWidget_3_doubleClicked(const QModelIndex &index)
{
    // populate the fields on the right
    QString uname = ui->tableWidget_3->model()->data(ui->tableWidget_3->model()->index(index.row(), 0)).toString();
    QString pw = ui->tableWidget_3->model()->data(ui->tableWidget_3->model()->index(index.row(), 1)).toString();
    QString role = ui->tableWidget_3->model()->data(ui->tableWidget_3->model()->index(index.row(), 2)).toString();
    qDebug() << uname;
    qDebug() << pw;
    qDebug() << role;

//    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableWidget_3->model());
//    int row = index.row();

//     QStandardItemModel* model = ui->tableWidget_3->model();
//    qDebug() << model;
//    QString uname = model->item(row, 0)->text();
//    QString pw = model->item(row, 1)->text();
//    QString role = model->item(row, 2)->text();

    if (role == "STANDARD") {
        ui->comboBox->setCurrentIndex(0);
    } else if (role == "CASE WORKER") {
        ui->comboBox->setCurrentIndex(1);
    } else if (role == "ADMIN") {
        ui->comboBox->setCurrentIndex(2);
    }

    ui->le_userName->setText(uname);
    ui->le_password->setText(pw);
}


void MainWindow::on_pushButton_CaseFiles_clicked()
{
    ui->stackedWidget->setCurrentIndex(CASEFILE);

    double width = ui->tw_pcpRela->size().width();

    for (auto x: pcp_tables){
        x->resizeRowsToContents();
        x->setColumnWidth(0, width*0.41);
        x->setColumnWidth(1, width*0.41);
        x->setColumnWidth(2, width*0.16);
    }
}

void MainWindow::on_EditRoomsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(EDITROOM);
}

void MainWindow::on_editUpdate_clicked()
{

    if(!checkNumber(ui->editCost->text()))
        return;

    ui->editLunch->isChecked() == true ? curBook->lunch = "YES" : curBook->lunch = "NO";
    ui->editWakeCheck->isChecked() == true ? curBook->wakeTime = ui->editWakeup->time().toString() :
            curBook->wakeTime = "NO";
    double updateBalance = curClient->balance - ui->editRefundAmt->text().toDouble();
    curBook->cost = ui->editCost->text().toDouble();
    if(!dbManager->updateBalance(updateBalance, curClient->clientId)){
        qDebug() << "Error inserting new balance";
        return;
    }
    ui->editOC->setText("Original Cost: " + QString::number(curBook->cost, 'f', 2));
    double qt = ui->editRefundAmt->text().toDouble();
    ui->editRefundAmt->setText("0.0");
    if(qt < 0){
        curBook->monthly = false;
    }
    curClient->balance = updateBalance;
    curBook->endDate = ui->editDate->date();
    curBook->stringEnd = curBook->endDate.toString(Qt::ISODate);

    updateBooking(*curBook);




}

double MainWindow::calcRefund(QDate old, QDate n){
    int updatedDays = n.toJulianDay() - old.toJulianDay();
    double cpd;
    double updatedCost;
    if(updatedDays > 0){
        //DO A POPUP ERROR HERE
        if(curBook->monthly){
            qDebug() << "NOT POSSIBLE -> is a monthly booking";
            ui->editDate->setDate(curBook->endDate);
            updatedDays = 0;
        }
        cpd = curBook->cost / (double)curBook->stayLength;
        //curBook->cost += updatedDays * cpd;
        return updatedDays * cpd;
    }
    else{
        //ERROR POPUP FOR INVALID DATe
        if(n < curBook->startDate){


            ui->editDate->setDate(curBook->startDate);
            updatedDays = curBook->stayLength * -1;

        }
        cpd = curBook->cost / (double)curBook->stayLength;
        updatedCost = cpd * (curBook->stayLength + updatedDays);

        if(curBook->monthly){
            double normalRate = dbManager->getRoomCost(curBook->room);
            updatedCost = normalRate * (curBook->stayLength + updatedDays);
            if(updatedCost > curBook->cost)
                return 0;

        }
        return (curBook->cost - updatedCost) * -1;
    }
}

bool MainWindow::checkNumber(QString num){
    int l = num.length();
    int period = 0;
    char copy[l];
    strcpy(copy, num.toStdString().c_str());
    for(int i = 0; i < num.length(); i++){
        if(copy[i] == '.'){
            if(period)
                return false;
            period++;
            continue;
        }

        if(!isdigit(copy[i]))
            return false;
    }
    return true;
}
bool MainWindow::updateBooking(Booking b){
    QString query;
    QString monthly;
    curBook->monthly == true ? monthly = "YES" : monthly = "NO";
    query = "UPDATE BOOKING SET " +
            QString("SpaceID = '") + b.room + "', " +
            QString("ProgramCode = '") + b.program + "', " +
            QString("Cost = '") + QString::number(b.cost) + + "', " +
            QString("EndDate = '") + b.stringEnd + "', " +
            QString("Lunch = '") + b.lunch + "', " +
            QString("Wakeup = '") + b.wakeTime + "', " +
            QString("Monthly = '") + monthly + "'" +
            QString(" WHERE BookingId = '") +
            b.bookID + "'";
    return dbManager->updateBooking(query);



}

void MainWindow::on_editDate_dateChanged(const QDate &date)
{
    qDebug() << "Edit date called";
    double refund = 0;
    double newCost;
    QString cost = ui->editCost->text();
    if(!checkNumber(cost)){
        qDebug() << "NON NUMBER";
        return;
    }
    //curBook->cost = QString::number(curBook->cost, 'f', 2).toDouble();

    refund = calcRefund(curBook->endDate, date);
    qDebug() << "REFUNDING" << refund;


    newCost = curBook->cost + refund;
    ui->editCost->setText(QString::number(newCost));



}

void MainWindow::on_editManagePayment_clicked()
{
    trans = new transaction();
    double owed;

    owed = ui->editRefundAmt->text().toDouble();
    bool type;
    owed <= 0 ? type = false : type = true;
    payment * pay = new payment(this, trans, curClient->balance, owed , curClient, curBook, type);
    pay->exec();
}

void MainWindow::on_editCost_textChanged(const QString &arg1)
{
    double newCost = ui->editCost->text().toDouble();
    double refund = ui->editCancel->text().toDouble();
    if(newCost < curBook->cost){
        ui->editRefundLabel->setText("Refund");
        double realRefund = newCost - curBook->cost + refund;
        if(realRefund > 0)
            realRefund = 0;
        ui->editRefundAmt->setText(QString::number(realRefund, 'f', 2));
    }
    else{
        ui->editRefundLabel->setText("Owed");
        ui->editRefundAmt->setText(QString::number(newCost - curBook->cost, 'f', 2));
    }
}

void MainWindow::on_editCancel_textChanged(const QString &arg1)
{
    double newCost = ui->editCost->text().toDouble();
    double refund = ui->editCancel->text().toDouble();
    if(newCost < curBook->cost){
        ui->editRefundLabel->setText("Refund");
        double realRefund = newCost - curBook->cost + refund;
        if(realRefund > 0)
            realRefund = 0;
        ui->editRefundAmt->setText(QString::number(realRefund, 'f', 2));
    }
    else{
        ui->editRefundLabel->setText("Owed");
        ui->editRefundAmt->setText(QString::number(newCost - curBook->cost, 'f', 2));
    }
}

void MainWindow::on_editRoom_clicked()
{

}

void MainWindow::on_pushButton_bookRoom_clicked()
{
    curClient = new Client();
    int nRow = ui->tableWidget_search_client->currentRow();

    curClientID = curClient->clientId = ui->tableWidget_search_client->item(nRow, 0)->text();
    curClient->fName =  ui->tableWidget_search_client->item(nRow, 1)->text();
    curClient->mName =  ui->tableWidget_search_client->item(nRow, 2)->text();
    curClient->lName =  ui->tableWidget_search_client->item(nRow, 3)->text();
    curClient->balance =  ui->tableWidget_search_client->item(nRow, 4)->text().toFloat();

    curClient->fullName = QString(curClient->fName + " " + curClient->mName + " " + curClient->lName);

/*
    qDebug()<<"ID: " << curClientID << curClient->clientId;
    qDebug()<<"NAME: " << curClient->fullName;
    qDebug()<<"Balance: " << curClient->balance;
*/
    ui->stackedWidget->setCurrentIndex(BOOKINGLOOKUP);

}

// update employee button
void MainWindow::on_pushButton_4_clicked()
{
    // obtain username and pw and role from UI
    QString uname = ui->le_userName->text();
    QString pw = ui->le_password->text();

    if (uname.length() == 0) {
        ui->lbl_editUserWarning->setText("Enter a Username");
        return;
    }

    if (pw.length() == 0) {
        ui->lbl_editUserWarning->setText("Enter a Password");
        return;
    }

    // first, check to make sure the username is taken
    QSqlQuery queryResults = dbManager->findUser(uname);
    int numrows1 = queryResults.numRowsAffected();

    if (numrows1 == 1) {
        QSqlQuery queryResults = dbManager->updateEmployee(uname, pw, ui->comboBox->currentText());
        int numrows = queryResults.numRowsAffected();

        if (numrows != 0) {
            ui->lbl_editUserWarning->setText("Employee Updated");
            QStandardItemModel * model = new QStandardItemModel(0,0);
            model->clear();
            ui->tableWidget_3->clear();
            ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidget_3->setColumnCount(3);
            ui->tableWidget_3->setRowCount(0);
            ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");

            ui->comboBox->setCurrentIndex(0);
            ui->le_userName->setText("");
            ui->le_password->setText("");
            ui->le_users->setText("");
        } else {
            ui->lbl_editUserWarning->setText("Something went wrong - Please try again");
        }

        return;
    } else {
        ui->lbl_editUserWarning->setText("Employee Not Found");
        return;
    }
}

// Clear button
void MainWindow::on_btn_displayUser_clicked()
{
    QStandardItemModel * model = new QStandardItemModel(0,0);
    model->clear();
    ui->tableWidget_3->clear();
    ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_3->setColumnCount(3);
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");

    ui->comboBox->setCurrentIndex(0);
    ui->le_userName->setText("");
    ui->le_password->setText("");
    ui->le_users->setText("");
}

void MainWindow::on_tableWidget_3_clicked(const QModelIndex &index)
{
    // populate the fields on the right
    QString uname = ui->tableWidget_3->model()->data(ui->tableWidget_3->model()->index(index.row(), 0)).toString();
    QString pw = ui->tableWidget_3->model()->data(ui->tableWidget_3->model()->index(index.row(), 1)).toString();
    QString role = ui->tableWidget_3->model()->data(ui->tableWidget_3->model()->index(index.row(), 2)).toString();
    qDebug() << uname;
    qDebug() << pw;
    qDebug() << role;

//    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->tableWidget_3->model());
//    int row = index.row();

//     QStandardItemModel* model = ui->tableWidget_3->model();
//    qDebug() << model;
//    QString uname = model->item(row, 0)->text();
//    QString pw = model->item(row, 1)->text();
//    QString role = model->item(row, 2)->text();

    if (role == "STANDARD") {
        ui->comboBox->setCurrentIndex(0);
    } else if (role == "CASE WORKER") {
        ui->comboBox->setCurrentIndex(1);
    } else if (role == "ADMIN") {
        ui->comboBox->setCurrentIndex(2);
    }

    ui->le_userName->setText(uname);
    ui->le_password->setText(pw);
}

// delete button
void MainWindow::on_pushButton_6_clicked()
{
    QString uname = ui->le_userName->text();
    QString pw = ui->le_password->text();

    if (uname.length() == 0) {
        ui->lbl_editUserWarning->setText("Please make sure a valid employee is selected");
        return;
    }

    if (pw.length() == 0) {
        ui->lbl_editUserWarning->setText("Please make sure a valid employee is selected");
        return;
    }

    QSqlQuery queryResults = dbManager->findUser(uname);
    int numrows1 = queryResults.numRowsAffected();

    if (numrows1 == 1) {
        QSqlQuery queryResults = dbManager->deleteEmployee(uname, pw, ui->comboBox->currentText());
        int numrows = queryResults.numRowsAffected();

        if (numrows != 0) {
            ui->lbl_editUserWarning->setText("Employee Deleted");
            QStandardItemModel * model = new QStandardItemModel(0,0);
            model->clear();
            ui->tableWidget_3->clear();
            ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidget_3->setColumnCount(3);
            ui->tableWidget_3->setRowCount(0);
            ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");

            ui->comboBox->setCurrentIndex(0);
            ui->le_userName->setText("");
            ui->le_password->setText("");
            ui->le_users->setText("");
        } else {
            ui->lbl_editUserWarning->setText("Employee Not Found");
        }

        return;
    } else {
        ui->lbl_editUserWarning->setText("Employee Not Found");
        return;
    }

}
void MainWindow::popManagePayment(){
    QStringList dropItems;
    ui->cbox_payDateRange->clear();

    dropItems << "" << "Today" << "Last 3 Days" << "This Month" <<  QDate::longMonthName(QDate::currentDate().month() - 1) << QDate::longMonthName(QDate::currentDate().month() - 2);
    ui->cbox_payDateRange->addItems(dropItems);
}

void MainWindow::on_cbox_payDateRange_activated(int index)
{
    QString startDate;
    QDate endDate = QDate::currentDate();
    QDate hold = QDate::currentDate();
    int days, move;
    switch(index){
    case 1:
        startDate = QDate::currentDate().toString(Qt::ISODate);
        break;
    case 2:
        hold = hold.addDays(-3);
        startDate = hold.toString(Qt::ISODate);
        break;
    case 3:
        days = hold.daysInMonth();
        days = days - hold.day();
        endDate = hold.addDays(days);
        move = hold.day() -1;
        hold = hold.addDays(move * -1);
        qDebug() << hold;
        break;
    case 4:
        hold = hold.addMonths(-1);
        days = hold.daysInMonth();

        move = hold.day() -1;
        days = days - hold.day();
        endDate = hold.addDays(days);
        hold = hold.addDays(move * -1);
        qDebug() << hold;
        break;
    case 5:
        hold = hold.addMonths(-2);
        days = hold.daysInMonth();

        move = hold.day() -1;
        days = days - hold.day();
        endDate = hold.addDays(days);
        hold = hold.addDays(move * -1);
        qDebug() << hold;
        break;
    }
    QStringList heads;
    QStringList cols;
    QSqlQuery tempSql = dbManager->getTransactions(hold, endDate);
    heads << "Date" << "Amount" << "Type" << "Method" << "Notes << MSQ";
    cols << "Date" << "Amount" << "TransType" << "Type" << "Notes" << "MSQ";
    populateATable(ui->mpTable, heads, cols, tempSql, false);

}

//PARAMS - The table, list of headers, list of table column names, the sqlquery result, STRETCH - stretch mode true/false
void MainWindow::populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch){
    table->clear();
    if(headers.length() != items.length())
        return;

    if(stretch)
        table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->hide();
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    int colCount = headers.size();
    table->setColumnCount(colCount);
    table->setHorizontalHeaderLabels(headers);
    int x = 0;
    while(result.next()){
        table->insertRow(x);
        for(int i = 0; i < colCount; i++){
            table->setItem(x, i, new QTableWidgetItem(result.value(items.at(i)).toString()));


        }


        x++;
    }
}

void MainWindow::on_btn_payListAllUsers_clicked()
{
    QStringList cols;
    QStringList heads;
    QDate sDate = QDate::fromString("1970-01-01", "yyyy-MM-dd");
    QDate eDate = QDate::fromString("2222-01-01", "yyyy-MM-dd");
    QSqlQuery tempSql = dbManager->getTransactions(sDate, eDate);
    heads << "Date" << "Amount" << "Type" << "Method" << "Notes" << "MSQ";
    cols << "Date" << "Amount" << "TransType" << "Type" << "Notes" << "MSQ";
    populateATable(ui->mpTable, heads, cols, tempSql, false);

// list all rooms
}
void MainWindow::on_btn_listAllUsers_3_clicked()
{

//    QString ename = ui->le_users->text();
//    ui->tableWidget_3->setRowCount(0);
//    ui->tableWidget_3->clear();
//    ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);

//    QSqlQuery result = dbManager->execQuery("SELECT Username, Password, Role FROM Employee WHERE Username LIKE '%"+ ename +"%'");

//    int numCols = result.record().count();
//    ui->tableWidget_3->setColumnCount(numCols);
//    ui->tableWidget_3->setHorizontalHeaderLabels(QStringList() << "Username" << "Password" << "Role");
//    int x = 0;
//    int qt = result.size();
//    qDebug() << qt;
//    while (result.next()) {
//        ui->tableWidget_3->insertRow(x);
//        QStringList row;
//        row << result.value(0).toString() << result.value(1).toString() << result.value(2).toString();
//        for (int i = 0; i < 3; ++i)
//        {
//            ui->tableWidget_3->setItem(x, i, new QTableWidgetItem(row.at(i)));
//        }
//        x++;
//    }
}

// list all programs
void MainWindow::on_btn_listAllUsers_2_clicked()
{
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->clear();
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);

    QSqlQuery result = dbManager->execQuery("SELECT ProgramCode, Description FROM Program");

    int numCols = result.record().count();
    ui->tableWidget_2->setColumnCount(numCols);
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "Program Code" << "Description");
    int x = 0;
    int qt = result.size();
    qDebug() << qt;
    while (result.next()) {
        ui->tableWidget_2->insertRow(x);
        QStringList row;
        row << result.value(0).toString() << result.value(1).toString();
        for (int i = 0; i < 2; ++i)
        {
            ui->tableWidget_2->setItem(x, i, new QTableWidgetItem(row.at(i)));
        }
        x++;
    }
}

// search programs by code
void MainWindow::on_btn_searchUsers_2_clicked()
{
    QString ename = ui->le_users_2->text();
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->clear();
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);

    QSqlQuery result = dbManager->execQuery("SELECT ProgramCode, Description FROM Program WHERE ProgramCode LIKE '%"+ ename +"%'");

    int numCols = result.record().count();
    ui->tableWidget_2->setColumnCount(numCols);
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "Program Code" << "Description");
    int x = 0;
    int qt = result.size();
    qDebug() << qt;
    while (result.next()) {
        ui->tableWidget_2->insertRow(x);
        QStringList row;
        row << result.value(0).toString() << result.value(1).toString();
        for (int i = 0; i < 2; ++i)
        {
            ui->tableWidget_2->setItem(x, i, new QTableWidgetItem(row.at(i)));
        }
        x++;
    }
}

// delete program
void MainWindow::on_pushButton_25_clicked()
{
    QString pcode = ui->le_userName_2->text();

    if (pcode.length() == 0) {
        ui->lbl_editUserWarning->setText("Please make sure a valid Program is selected");
        return;
    }

    QSqlQuery queryResults = dbManager->execQuery("DELETE FROM Program WHERE ProgramCode='" + pcode + "'");
    int numrows = queryResults.numRowsAffected();

    if (numrows != 0) {
        ui->lbl_editProgWarning->setText("Program Deleted");
        QStandardItemModel * model = new QStandardItemModel(0,0);
        model->clear();
        ui->tableWidget_2->clear();
        ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);
        ui->tableWidget_2->setColumnCount(2);
        ui->tableWidget_2->setRowCount(0);
        ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "Program Code" << "Description");

        ui->comboBox->setCurrentIndex(0);
        ui->le_userName->setText("");
        ui->le_password->setText("");
        ui->le_users->setText("");
    } else {
        ui->lbl_editProgWarning->setText("Program Not Found");
    }
    return;
}

// program clicked + selected
void MainWindow::on_tableWidget_2_clicked(const QModelIndex &index)
{
    // populate the fields on the right
    QString pcode = ui->tableWidget_2->model()->data(ui->tableWidget_2->model()->index(index.row(), 0)).toString();
    QString description = ui->tableWidget_2->model()->data(ui->tableWidget_2->model()->index(index.row(), 1)).toString();

    ui->le_userName_2->setText(pcode);
    ui->textEdit->setText(description);
}

// set case files directory
void MainWindow::on_pushButton_3_clicked()
{
    QString tempDir = QFileDialog::getExistingDirectory(
                    this,
                    tr("Select Directory"),
                    "C://"
                );
//    qDebug() << curClientID;
    if (!tempDir.isEmpty()) {
        dir = tempDir;
        populate_tw_caseFiles();
        ui->le_caseDir->setText(dir.path());
    }
    connect(ui->tw_caseFiles, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(on_tw_caseFiles_cellDoubleClicked(int,int)), Qt::UniqueConnection);
}

// open case file in external reader
void MainWindow::on_tw_caseFiles_cellDoubleClicked(int row, int column)
{
    qDebug() << QUrl::fromLocalFile(dir.absoluteFilePath(ui->tw_caseFiles->item(row, column)->text())) << "at" << row << " " << column;
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absoluteFilePath(ui->tw_caseFiles->item(row, column)->text())));
}

// filter file names
void MainWindow::on_btn_caseFilter_clicked()
{
    qDebug() << "filter button clicked, filter with" << ui->le_caseFilter->text();
    QStringList filter = (QStringList() << "*"+(ui->le_caseFilter->text())+"*");
    populate_tw_caseFiles(filter);
}

void MainWindow::populate_tw_caseFiles(QStringList filter){
    int i = 0;
    dir.setNameFilters(filter);
    for (auto x : dir.entryList(QDir::Files)) {
        qDebug() << x;
        ui->tw_caseFiles->setRowCount(i+1);
        ui->tw_caseFiles->setItem(i++,0, new QTableWidgetItem(x));
        ui->tw_caseFiles->resizeColumnsToContents();
    }
    if (i > 0) {
        ui->btn_caseFilter->setEnabled(true);
    }

}

// create new program button
void MainWindow::on_btn_createNewUser_2_clicked()
{
    QString pcode = ui->le_userName_2->text();
    QString pdesc = ui->textEdit->toPlainText();

    if (pcode.length() == 0) {
        ui->lbl_editProgWarning->setText("Please Enter a Program Code");
        return;
    }

    if (pdesc.length() == 0) {
        ui->lbl_editProgWarning->setText("Please Enter a Program Description");
        return;
    }

    QSqlQuery queryResults = dbManager->execQuery("SELECT * FROM Program WHERE ProgramCode='" + pcode + "'");
    int numrows = queryResults.numRowsAffected();

    if (numrows == 1) {
        ui->lbl_editProgWarning->setText("Program code in use");
        return;
    } else {
        QSqlQuery qr = dbManager->AddProgram(pcode, pdesc);
        if (qr.numRowsAffected() == 1) {
            ui->lbl_editProgWarning->setText("Program Added");
            QStandardItemModel * model = new QStandardItemModel(0,0);
            model->clear();
            ui->tableWidget_2->clear();
            ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidget_2->setColumnCount(2);
            ui->tableWidget_2->setRowCount(0);
            ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "Program Code" << "Description");

            ui->comboBox->setCurrentIndex(0);
            ui->le_userName->setText("");
            ui->le_password->setText("");
            ui->le_users->setText("");
        } else {
            ui->lbl_editProgWarning->setText("Program not added - please try again.");
        }
    }
}

// update program
void MainWindow::on_pushButton_24_clicked()
{
    QString pcode = ui->le_userName_2->text();
    QString pdesc = ui->textEdit->toPlainText();

    if (pcode.length() == 0) {
        ui->lbl_editProgWarning->setText("Please Enter a Program Code");
        return;
    }

    if (pdesc.length() == 0) {
        ui->lbl_editProgWarning->setText("Please Enter a Program Description");
        return;
    }

    QSqlQuery queryResults = dbManager->execQuery("SELECT * FROM Program WHERE ProgramCode='" + pcode + "'");
    int numrows = queryResults.numRowsAffected();

    if (numrows != 1) {
        ui->lbl_editProgWarning->setText("Enter a valid program code to update");
        return;
    } else {
        QSqlQuery qr = dbManager->updateProgram(pcode, pdesc);
        if (qr.numRowsAffected() == 1) {
            ui->lbl_editProgWarning->setText("Program Updated");
            QStandardItemModel * model = new QStandardItemModel(0,0);
            model->clear();
            ui->tableWidget_2->clear();
            ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidget_2->setColumnCount(2);
            ui->tableWidget_2->setRowCount(0);
            ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "Program Code" << "Description");

            ui->comboBox->setCurrentIndex(0);
            ui->le_userName->setText("");
            ui->le_password->setText("");
            ui->le_users->setText("");
        } else {
            ui->lbl_editProgWarning->setText("Program not updated - please try again.");
        }
    }
}


void MainWindow::resizeEvent(QResizeEvent *event) {
    double width = ui->tw_pcpRela->size().width();
    for (auto x: pcp_tables){
        x->resizeRowsToContents();
        x->setColumnWidth(0, width*0.41);
        x->setColumnWidth(1, width*0.41);
        x->setColumnWidth(2, width*0.16);
    }
}

void MainWindow::on_tw_pcpRela_itemChanged(QTableWidgetItem *item)
{
    for (auto x: pcp_tables){
        x->resizeRowsToContents();
    }
}

void MainWindow::setPcpVector(){
    pcp_tables.push_back(ui->tw_pcpAcco);
    pcp_tables.push_back(ui->tw_pcpAct);
    pcp_tables.push_back(ui->tw_pcpEdu);
    pcp_tables.push_back(ui->tw_pcpLeg);
    pcp_tables.push_back(ui->tw_pcpLife);
    pcp_tables.push_back(ui->tw_pcpMent);
    pcp_tables.push_back(ui->tw_pcpOther);
    pcp_tables.push_back(ui->tw_pcpPhy);
    pcp_tables.push_back(ui->tw_pcpPpl);
    pcp_tables.push_back(ui->tw_pcpRela);
    pcp_tables.push_back(ui->tw_pcpSub);
    pcp_tables.push_back(ui->tw_pcpTrad);
}

void MainWindow::on_btn_pcpRela_clicked()
{
    ui->tw_pcpRela->insertRow(0);
    ui->tw_pcpRela->setMinimumHeight(ui->tw_pcpRela->minimumHeight()+35);
}

void MainWindow::on_btn_pcpEdu_clicked()
{
    ui->tw_pcpEdu->insertRow(0);
    ui->tw_pcpEdu->setMinimumHeight(ui->tw_pcpEdu->minimumHeight()+35);
}

void MainWindow::on_btn_pcpSub_clicked()
{
    ui->tw_pcpSub->insertRow(0);
    ui->tw_pcpSub->setMinimumHeight(ui->tw_pcpSub->minimumHeight()+35);
}

void MainWindow::on_btn_pcpAcc_clicked()
{
    ui->tw_pcpAcco->insertRow(0);
    ui->tw_pcpAcco->setMinimumHeight(ui->tw_pcpAcco->minimumHeight()+35);
}

void MainWindow::on_btn_pcpLife_clicked()
{
    ui->tw_pcpLife->insertRow(0);
    ui->tw_pcpLife->setMinimumHeight(ui->tw_pcpLife->minimumHeight()+35);
}

void MainWindow::on_btn_pcpMent_clicked()
{
    ui->tw_pcpMent->insertRow(0);
    ui->tw_pcpMent->setMinimumHeight(ui->tw_pcpMent->minimumHeight()+35);
}

void MainWindow::on_btn_pcpPhy_clicked()
{
    ui->tw_pcpPhy->insertRow(0);
    ui->tw_pcpPhy->setMinimumHeight(ui->tw_pcpPhy->minimumHeight()+35);
}

void MainWindow::on_btn_pcpLeg_clicked()
{
    ui->tw_pcpLeg->insertRow(0);
    ui->tw_pcpLeg->setMinimumHeight(ui->tw_pcpLeg->minimumHeight()+35);
}

void MainWindow::on_btn_pcpAct_clicked()
{
    ui->tw_pcpAct->insertRow(0);
    ui->tw_pcpAct->setMinimumHeight(ui->tw_pcpAct->minimumHeight()+35);
}

void MainWindow::on_btn_pcpTrad_clicked()
{
    ui->tw_pcpTrad->insertRow(0);
    ui->tw_pcpTrad->setMinimumHeight(ui->tw_pcpTrad->minimumHeight()+35);
}

void MainWindow::on_btn_pcpOther_clicked()
{
    ui->tw_pcpOther->insertRow(0);
    ui->tw_pcpOther->setMinimumHeight(ui->tw_pcpOther->minimumHeight()+35);
}

void MainWindow::on_btn_pcpKey_clicked()
{
    ui->tw_pcpPpl->insertRow(0);
    ui->tw_pcpPpl->setMinimumHeight(ui->tw_pcpPpl->minimumHeight()+35);
}
