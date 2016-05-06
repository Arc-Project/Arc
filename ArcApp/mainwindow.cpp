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

void MainWindow::on_pushButton_RegisterClient_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->dateEdit_cl_rulesign->setDate(QDate::currentDate());
}

void MainWindow::on_button_cancel_client_register_clicked()
{
    clear_client_register_form();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_reportsButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(11);
}

//Client Regiter widget [TAKE A PICTURE] button
void MainWindow::on_button_cl_takePic_clicked()
{
    TakePhoto *camDialog = new TakePhoto();

    connect(camDialog, SIGNAL(showPic(QImage)), this, SLOT(addPic(QImage)));
    camDialog->show();
}

//add picture into graphicview (after taking picture in pic dialog
void MainWindow::addPic(QImage pict){

  //  qDebug()<<"ADDPIC";

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
    ui->graphicsView_cl_pic->setScene(scene);

    //delete picture function to database

}

//search client
void MainWindow::on_pushButton_search_client_clicked()
{

       qDebug() <<"START SEARCH CLIENT";
    QString clientName = ui->lineEdit_search_clientName->text();
    QString searchQuery = "SELECT FirstName, LastName, Dob FROM Client WHERE LastName LIKE '%"+clientName+"%' OR FirstName Like '%"+clientName+"%'";

    QSqlQuery results = dbManager->execQuery(searchQuery);
    setup_searchClientTable(results);
    dbManager->printAll(results);

}

void MainWindow::setup_searchClientTable(QSqlQuery query){
    QSqlQueryModel *clientModel = new QSqlQueryModel();
    clientModel->setQuery(query);
    ui->tableView_search_client->setModel(clientModel);
    qDebug()<< clientModel->rowCount();
}

//Client information input and register click
void MainWindow::on_button_register_client_clicked()
{
    if(check_client_register_form()){
        qDebug()<<ui->lineEdit_cl_fName->text();
        qDebug()<<ui->lineEdit_cl_mName->text();
        qDebug()<<ui->lineEdit_cl_lName->text();
        bool parolee;
        bool allowComm = ui->checkBox_cl_comm->isChecked();
        if(parolee = ui->checkBox_cl_parolee->isChecked())
            qDebug()<<"parolee is checked : " << QString::number(parolee);
        else
            qDebug()<<"parolee is not checked : " << parolee;
        qDebug()<<"DATE function : "<<ui->dateEdit_cl_dob->date().toString("yyyy-MM-dd");
        dbManager->execQuery("INSERT INTO Client (FirstName, MiddleName, LastName, Dob, Balance, SinNo, GaNo, IsParolee, AllowComm, DateRulesSigned, Status) VALUES ('"
                             + ui->lineEdit_cl_fName->text()+"', '"
                             + ui->lineEdit_cl_mName->text()+"', '"
                             + ui->lineEdit_cl_lName->text()+"', '"
                             + ui->dateEdit_cl_dob->date().toString("yyyy-MM-dd") //+"', '"
                             + "',DEFAULT,'"
                             + ui->lineEdit_cl_SIN->text()+"', '"
                             + ui->lineEdit_cl_GANum->text()+"', "
                             + QString::number(parolee) + ","
                             + QString::number(allowComm)+ ", '"
                             // + (ui->checkBox_cl_parolee->isChecked()?1:0)+", "
                          //   + (ui->checkBox_cl_comm->isChecked()?1:0)+", '"
                             + ui->dateEdit_cl_rulesign->date().toString("yyyy-MM-dd")
                             +"',DEFAULT)");
                             //+"DEFAULT)");
//        qDebug()<<"    CHECK parolee " + (ui->checkBox_cl_parolee->isChecked()?1:0);
        qDebug()<<"REGISTER FINISHED";
        clear_client_register_form();
        ui->stackedWidget->setCurrentIndex(1);
    }
}
void MainWindow::on_button_clear_client_regForm_clicked()
{
    clear_client_register_form();
}

void MainWindow::clear_client_register_form(){
    ui->lineEdit_cl_fName->clear();
    ui->lineEdit_cl_mName->clear();
    ui->lineEdit_cl_lName->clear();
    ui->lineEdit_cl_SIN->clear();
    ui->lineEdit_cl_GANum->clear();
    ui->lineEdit_cl_nok_name->clear();
    ui->lineEdit_cl_nok_relationship->clear();
    ui->lineEdit_cl_nok_loc->clear();
    ui->lineEdit_cl_nok_ContactNo->clear();
    ui->lineEdit_cl_phys_name->clear();
    ui->lineEdit_cl_phys_ContactNo->clear();
    ui->lineEdit_cl_Msd_Name->clear();
    ui->lineEdit_cl_Msd_ContactNo->clear();
    ui->plainTextEdit_cl_comments->clear();
    QDate defaultDob= QDate::fromString("1990-01-01","yyyy-MM-dd");
    ui->dateEdit_cl_dob->setDate(defaultDob);
    ui->dateEdit_cl_rulesign->setDate(QDate::currentDate());
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

    payment * pay = new payment(this, trans, 500.0, 30.0, curClient);
    pay->exec();
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
