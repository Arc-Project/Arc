#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "databasemanager.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_testDbConnBtn_clicked();

    void on_testQueryBtn_clicked();


    void on_bookButton_clicked();

    void on_clientButton_clicked();

    void on_paymentButton_clicked();

    void on_editbookButton_clicked();

    void on_pushButton_clicked();

    void on_caseButton_clicked();

    void on_adminButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
