#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAINMENU 0
#define CLIENTLOOKUP 1
#define BOOKINGLOOKUP 2
#define BOOKINGPAGE 3
#define PAYMENTPAGE 4
#define ADMINPAGE 5
#define EDITUSERS 6
#define EDITPROGRAM 7
#define CASEFILE 8
#define EDITBOOKING 9
#include <QMainWindow>
#include <QDebug>
#include "databasemanager.h"
#include "bookingmanager.h"
#include <QTableWidgetItem>
#include "booking.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bookingManager book;
    void getProgramCodes();
    void bookingSetup();
    void populateBooking();
    void setBooking(int row);
private slots:

    void on_bookButton_clicked();

    void on_clientButton_clicked();

    void on_paymentButton_clicked();

    void on_editbookButton_clicked();

    void on_caseButton_clicked();

    void on_adminButton_clicked();

    void on_bookingSearchButton_clicked();

    void on_makeBookingButton_clicked();

    void on_EditUserButton_clicked();

    void on_EditProgramButton_clicked();

    void on_actionMain_Menu_triggered();

    void on_actionDB_Connection_triggered();

    void on_bookingTable_itemClicked(QTableWidgetItem *item);

    void on_makeBookingButton_2_clicked();

    void on_monthCheck_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    MainWindow * mw;
    Booking * curBook;
};

#endif // MAINWINDOW_H
