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

    void on_bookButton_clicked();

    void on_clientButton_clicked();

    void on_paymentButton_clicked();

    void on_editbookButton_clicked();

    void on_caseButton_clicked();

    void on_adminButton_clicked();

    void on_sCalButton_clicked();

    void on_eCalButton_clicked();

    void on_startCalendar_clicked(const QDate &date);

    void on_endCalendar_clicked(const QDate &date);

    void on_bookingSearchButton_clicked();

    void on_makeBookingButton_clicked();

    void on_EditUserButton_clicked();

    void on_EditProgramButton_clicked();

    void on_actionMain_Menu_triggered();

    void on_actionDB_Connection_triggered();

    void on_pushButton_RegisterClient_clicked();

    void on_button_cancle_Register_clicked();

    void on_button_cl_takePic_clicked();

    void addPic(QImage pict);


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
