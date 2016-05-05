#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "takephoto.h"

#include <QTableView>
#include <QItemDelegate>
#include <QStandardItemModel>
#include <QDebug>

bool sCal = false;
bool eCal = false;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->stackedWidget);

    ui->startCalendar->hide();
    ui->endCalendar->hide();
    ui->listWidget->hide();
    ui->makeBookingButton->hide();
    //mw = this;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bookButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_clientButton_clicked()
{
     ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_paymentButton_clicked()
{
     ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_editbookButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(9);

}

void MainWindow::on_caseButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);

}

void MainWindow::on_adminButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);

}

void MainWindow::on_sCalButton_clicked()
{
    QDateTimeEdit *q = new QDateTimeEdit;
    q->calendarPopup();
    ui->listWidget->hide();
    if(sCal){
        ui->startCalendar->hide();
        qDebug() << "Hiding calendar";
        sCal = false;
    }
    else{
        ui->startCalendar->show();
        qDebug() << "Showing calendar";
        sCal = true;
    }

}

void MainWindow::on_eCalButton_clicked()
{
    ui->listWidget->hide();
    if(eCal){
        ui->endCalendar->hide();
        qDebug() << "Hiding calendar";
        eCal = false;
    }
    else{
        ui->endCalendar->show();
        qDebug() << "Showing calendar";
        eCal = true;
    }
}

void MainWindow::on_startCalendar_clicked(const QDate &date)
{
    ui->startDateEdit->setDate(date);
    ui->startCalendar->hide();
    sCal = false;

}

void MainWindow::on_endCalendar_clicked(const QDate &date)
{
    ui->endDateEdit->setDate(date);
    ui->endCalendar->hide();
    eCal = false;
}

void MainWindow::on_bookingSearchButton_clicked()
{
    ui->startCalendar->hide();
    ui->endCalendar->hide();
    ui->makeBookingButton->show();
    ui->listWidget->show();
    eCal = false;
    sCal = false;
}

void MainWindow::on_makeBookingButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);

}

void MainWindow::on_EditUserButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}

void MainWindow::on_EditProgramButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);

}

void MainWindow::on_actionMain_Menu_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_actionDB_Connection_triggered()
{
    QSqlQuery results= dbManager->selectAll("Test");
    dbManager->printAll(results);
}

void MainWindow::on_actionFile_Upload_triggered()
{
    QString strFilePath = MainWindow::browse();
    if (!strFilePath.isEmpty())
    {
        if (dbManager->uploadCaseFile(strFilePath))
        {
            qDebug() << "Case file uploaded";
        }
        else
        {
            qDebug() << "Could not upload case file";
        }
    }
    else
    {
        qDebug() << "Empty file path";
    }
}

QString MainWindow::browse()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::DirectoryOnly);
    QString strFilePath = dialog.getOpenFileName(this, tr("SelectFile"), "", tr("All Files (*.*)"));

    return strFilePath;
}

void MainWindow::on_actionDownload_Latest_Upload_triggered()
{
    if (dbManager->downloadLatestCaseFile())
    {
        qDebug() << "file downloaded";
    }
    else
    {
        qDebug() << "could not download file";
    }
}

void MainWindow::on_pushButton_RegisterClient_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    ui->dateEdit_cl_rulesign->setDate(QDate::currentDate());
}

void MainWindow::on_button_cancle_Register_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
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



//search client
void MainWindow::on_pushButton_search_client_clicked()
{

       qDebug() <<"START SEARCH CLIENT";
    QString clientName = ui->lineEdit_search_clientName->text();
    QString searchQuery = "SELECT FirstName, LastName, Dob FROM Client WHERE LastName LIKE '%"+clientName+"%' OR FirstName Like '%"+clientName+"%'";


    QSqlQuery results = dbManager->execQuery(searchQuery);
    dbManager->printAll(results);

    //dbManager->execQuery("INSERT INTO Client (FirstName, MiddleName, LastName, Dob) VALUES ('test',NULL, 'testsur', '2000-10-10'");

}

void MainWindow::setup_searchClientTable(QSqlQuery query){
    QStandardItemModel *clientSearchModel = new QStandardItemModel(3,20,this);
    ui->tableView_search_client->setModel(clientSearchModel);
    for(int row = 0; row <20; row++){
       // for(int col =0; col < 3; col++){
            QModelIndex col0 = clientSearchModel->index(row, 0, QModelIndex());
            QModelIndex col1 = clientSearchModel->index(row, 0, QModelIndex());
            QModelIndex col2 = clientSearchModel->index(row, 0, QModelIndex());

            clientSearchModel->setData(col0,query.value("FirstName"));
            clientSearchModel->setData(col1,query.value("LastName"));
            clientSearchModel->setData(col2,query.value("Dob"));
       // }
    }
}

//Client information input and register click
void MainWindow::on_button_register_client_clicked()
{
    if(check_register_form()){
        qDebug()<<ui->lineEdit_cl_fName->text();
        qDebug()<<ui->lineEdit_cl_mName->text();
        qDebug()<<ui->lineEdit_cl_lName->text();
        qDebug()<<"DATE function : "<<ui->dateEdit_cl_dob->date().toString("yyyy-MM-dd");
        dbManager->execQuery("INSERT INTO Client (FirstName, MiddleName, LastName, Dob, Balance) VALUES ('"
                             + ui->lineEdit_cl_fName->text()+"', '"
                             + ui->lineEdit_cl_mName->text()+"', '"
                             + ui->lineEdit_cl_lName->text()+"', '"
                             + ui->dateEdit_cl_dob->date().toString("yyyy-MM-dd")
                             + "', DEFAULT)");
        qDebug()<<"REGISTER FINISHED";
        ui->stackedWidget->setCurrentIndex(1);
    }

}

//check if the value is valid or not
bool MainWindow::check_register_form(){
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
