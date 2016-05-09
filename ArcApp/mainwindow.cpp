#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "takephoto.h"
#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QDebug>
#include "payment.h"

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
    QSqlQuery results= dbManager->selectAll("Test");
    dbManager->printAll(results);
}

void MainWindow::on_actionTest_Query_triggered()
{
    QStringList fieldList;
    getListRegisterFields(&fieldList);

    for(int i = 0; i < fieldList.size(); ++i)
    {
        qDebug() << fieldList.at(i);
    }

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
    ui->stayLabel->setText(QString::number(curClient->balance - curBook->cost + curBook->paidTotal, 'f', 2));
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

    if(!dbManager->insertBookingTable(values)){
        qDebug() << "ERROR INSERTING BOOKING";
    }
    for(int i = 1; i < curBook->stayLength; i++){
        QDate n = next.addDays(i);
        values = "'" + today.toString(Qt::ISODate) + "','" + n.toString(Qt::ISODate) + "','" + curBook->room + "','" +
                 curBook->clientId + "','" + curBook->program + "','" + QString::number(cost) + "','" + curBook->stringStart
                 + "','" + curBook->stringEnd + "','" + curBook->lunch + "','" + curBook->wakeTime + "'," + "'NO'" + ",'" + month + "','" + "Eunwon'";
        if(!dbManager->insertBookingTable(values)){
            qDebug() << "ERROR INSERTING BOOKING";
        }
    }
    curBook->cost = cost;
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
    ui->confirmPaid->setText(QString::number(curBook->paidTotal));
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
    MainWindow::updateCheckoutView();
    MainWindow::updateVacancyView();
    MainWindow::updateLunchView();
    MainWindow::updateWakeupView();
    ui->stackedWidget->setCurrentIndex(11);
}

/*===================================================================
  registration page
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
               << ui->comboBox_cl_status->currentText()//"green" //grab value from status dropdown
               << ui->plainTextEdit_cl_comments->toPlainText();

    qDebug()<<"get item from combobox 1: "<<ui->comboBox_cl_caseWorker->currentText() << "2: " <<ui->comboBox_cl_status->currentText();
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

//Client information input and register click
void MainWindow::on_button_register_client_clicked()
{
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
    if(ui->lineEdit_cl_fName->text().isEmpty()){
        ui->lineEdit_cl_fName->cursor();
        qDebug()<< "NameIsEmpty";
        return false;
    }
    else if(ui->lineEdit_cl_lName->text().isEmpty()){
        ui->lineEdit_cl_lName->cursor();
        qDebug()<<" Last Name Empty";
        return false;
    }
    else if(ui->dateEdit_cl_dob->date() == QDate::currentDate()){
        ui->dateEdit_cl_dob->cursor();
        qDebug()<<"Wrong Date";
        return false;
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
    QString searchQuery = "SELECT ClientId, FirstName, LastName, Dob FROM Client WHERE LastName LIKE '%"+clientName+"%' OR FirstName Like '%"+clientName+"%'";

    QSqlQuery results = dbManager->execQuery(searchQuery);
    setup_searchClientTable(results);
    dbManager->printAll(results);

    QItemSelectionModel *selectedCl = ui->tableView_search_client->selectionModel();
    connect(selectedCl, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selected_client_info(QModelIndex,QModelIndex)));


}

//get client information after searching
void MainWindow::selected_client_info(QModelIndex idx1,QModelIndex idx2)
{
    if(!pic_available || !table_available)
        return;


    QModelIndex data = idx1.sibling(idx1.row(), 0);
    QString val = data.data().toString();
    qDebug()<<"GET DATA:" << val;
    QString getInfoQuery = "SELECT FirstName, MiddleName, LastName, Dob, Balance, SinNo, GaNo, IsParolee, AllowComm, DateRulesSigned FROM Client WHERE ClientId = "+ val;
    QSqlQuery resultQ = dbManager->execQuery(getInfoQuery);

/*
    pic_available = false;
    QtConcurrent::run(this, &displayPicThread, val);
   table_available = false;
    QtConcurrent::run(this, &displayClientInfoThread, val);
*/
    qDebug()<<"Finish Select Query to tableview";

}

void MainWindow::clientSearchedInfo(){

    QGraphicsScene *scene = new QGraphicsScene();
    scene->clear();
    ui->graphicsView_getInfo->setScene(scene);
}

void MainWindow::displayClientInfoThread(QString val){
    QString getInfoQuery = "SELECT FirstName, MiddleName, LastName, Dob, Balance, SinNo, GaNo, IsParolee, AllowComm, DateRulesSigned FROM Client WHERE ClientId = "+ val;
    QSqlQuery resultQ = dbManager->execQuery(getInfoQuery);
    QSqlQueryModel *clientModel2 = new QSqlQueryModel();
    clientModel2->setQuery(resultQ);
    ui->tableView_clientInfo->setModel(clientModel2);
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



void MainWindow::setup_searchClientTable(QSqlQuery query){
    QSqlQueryModel *clientModel = new QSqlQueryModel();
    clientModel->setQuery(query);
    clientModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    clientModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Birthday"));
    ui->tableView_search_client->setColumnWidth(0,0);
    ui->tableView_search_client->setModel(clientModel);
    qDebug()<< clientModel->rowCount();


}









void MainWindow::on_paymentButton_2_clicked()
{
    trans = new transaction();
    double owed;
    //owed = curBook->cost;
    curClient = new Client();
    curClient->balance = 50.0;
    curClient->clientId = "1";
    curClient->fName = "Spenser";
    curClient->mName ="Joseph";
    curClient->lName = "Lee";
    curClient->fullName = "Spenser Joseph Lee";
    owed = ui->costInput->text().toDouble();
    payment * pay = new payment(this, trans, curClient->balance, owed , curClient, curBook);
    pay->exec();
    ui->stayLabel->setText(QString::number(curClient->balance - curBook->cost + curBook->paidTotal, 'f', 2));
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
                                                 << "Lunch" << "Wakeup");
    QSqlQuery result;
    QString user = "";
    if(ui->editClient->text() != ""){
        user = ui->editClient->text();
    }
    result = dbManager->getActiveBooking(user, true);
    int numCols = result.record().count();
    //dbManager->printAll(result);
    int x = 0;
    while (result.next()) {
        ui->editLookupTable->insertRow(x);
        QStringList row;
        row << result.value(1).toString() << result.value(7).toString() << result.value(8).toString() << result.value(12).toString()
            << result.value(3).toString() << result.value(13).toString() << result.value(5).toString() << result.value(6).toString()
                  << result.value(9).toString() << result.value(10).toString();
        for (int i = 0; i < 10; ++i)
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
    delete(curBook);
    delete(curClient);
    delete(trans);
    ui->stackedWidget->setCurrentIndex(MAINMENU);
}


void MainWindow::on_editButton_clicked()
{
    curBook = new Booking();
    popBookFromRow();
}
void MainWindow::popBookFromRow(){
    int row = ui->editLookupTable->selectionModel()->currentIndex().row();
    if(row == - 1){
        return;
    }
    curBook->cost = ui->editLookupTable->item(row,7)->text().toDouble();
    curBook->startDate = QDate::fromString(ui->editLookupTable->item(row, 1)->text(), "yyyy-MM-dd)");
    curBook->endDate = QDate::fromString(ui->editLookupTable->item(row, 2)->text(), "yyyy-MM-dd)");
    curBook->lunch = ui->editLookupTable->item(row,8)->text();
    if(ui->editLookupTable->item(row,3)->text() == "YES"){
      curBook->monthly = true;
         }
     else{
         curBook->monthly = false;
     }

    curBook->program = ui->editLookupTable->item(row,6)->text();
    curBook->room = ui->editLookupTable->item(row,4)->text();
    curBook->wakeTime = ui->editLookupTable->item(row,9)->text();
}

void MainWindow::on_btn_listAllUsers_clicked()
{
    QSqlQuery query = dbManager->execQuery("SELECT Username, Password, Role FROM Employee");
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(query);
    ui->tableView_3->setModel(model);
    ui->tableView_3->horizontalHeader()->model()->setHeaderData(0, Qt::Horizontal, "Username");
    ui->tableView_3->horizontalHeader()->model()->setHeaderData(1, Qt::Horizontal, "Password");
    ui->tableView_3->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, "Role");
}

void MainWindow::on_btn_searchUsers_clicked()
{
    QString ename = ui->le_users->text();
    QSqlQuery results = dbManager->execQuery("SELECT Username, Password, Role FROM Employee WHERE Username LIKE '%"+ ename +"%'");
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(results);
    ui->tableView_3->setModel(model);
    ui->tableView_3->horizontalHeader()->model()->setHeaderData(0, Qt::Horizontal, "Username");
    ui->tableView_3->horizontalHeader()->model()->setHeaderData(1, Qt::Horizontal, "Password");
    ui->tableView_3->horizontalHeader()->model()->setHeaderData(2, Qt::Horizontal, "Role");
}

// double clicked employee
void MainWindow::on_tableView_3_doubleClicked(const QModelIndex &index)
{
    // populate the fields on the right
}


void MainWindow::on_pushButton_CaseFiles_clicked()
{
    ui->stackedWidget->setCurrentIndex(CASEFILE);
}

void MainWindow::on_EditRoomsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(EDITROOM);
}
