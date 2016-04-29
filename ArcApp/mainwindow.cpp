#include "mainwindow.h"
#include "ui_mainwindow.h"
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
