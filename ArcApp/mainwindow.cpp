#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_testDbConnBtn_clicked()
{
    QSqlQuery results= dbManager->selectAll("Test");
    dbManager->printAll(results);
}

void MainWindow::on_testQueryBtn_clicked()
{
}
