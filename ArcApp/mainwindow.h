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
#include <QtConcurrent/QtConcurrent>
#include "databasemanager.h"
#include "bookingmanager.h"
#include <QTableWidgetItem>
#include "booking.h"
#include "transaction.h"
#include "client.h"
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
    void setup_searchClientTable(QSqlQuery query);


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

    void on_makeBookingButton_2_clicked();

    void on_monthCheck_stateChanged(int arg1);
    void on_actionFile_Upload_triggered();

    QString browse();

    void on_actionDownload_Latest_Upload_triggered();

    void on_actionPrint_Db_Connections_triggered();

    void on_pushButton_RegisterClient_clicked();

    void on_button_cancel_client_register_clicked();

    void on_button_cl_takePic_clicked();

    void addPic(QImage pict);

    void on_reportsButton_clicked();

    void on_pushButton_search_client_clicked();

    void on_button_register_client_clicked();

    bool check_client_register_form();

    void clear_client_register_form();

    void on_paymentButton_2_clicked();

    void on_button_cl_delPic_clicked();

   // void on_pushButton_7_clicked();


    void on_btn_createNewUser_clicked();

    void on_button_clear_client_regForm_clicked();


private:
    Ui::MainWindow *ui;
    MainWindow * mw;
    Booking * curBook;
    transaction * trans;
    Client * curClient;
};

#endif // MAINWINDOW_H
