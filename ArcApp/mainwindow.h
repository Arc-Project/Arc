#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAINMENU            0
#define CLIENTLOOKUP        1
#define BOOKINGLOOKUP       2
#define BOOKINGPAGE         3
#define PAYMENTPAGE         4
#define ADMINPAGE           5
#define EDITUSERS           6
#define EDITPROGRAM         7
#define CASEFILE            8
#define EDITBOOKING         9
#define CLIENTREGISTER      10
#define REPORTS             11
#define EDITROOM            12
#define CONFIRMBOOKING      13
#define EDITPAGE            14
#define EDITSHIFT           15
#define DAILYREPORT         0
#define SHIFTREPORT         1
#define DAILYLOG            2
#define FLOATCOUNT          3
#define MONTHLYREPORT       4
#define RESTRICTIONS        5
#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "databasemanager.h"
#include "bookingmanager.h"
#include <QTableWidgetItem>
#include <QMap>
#include "booking.h"
#include "transaction.h"
#include "client.h"
#include "shared.h"
#include "casefile.h"
#include "Utility.h"
#include "mymodel.h"
#include "report.h"
#include "mycalendar.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString userLoggedIn = "SOMEUSER";
    int currentshiftid = 0;

    bookingManager book;
    void setup_searchClientTable(QSqlQuery results);
    void displayClientInfoThread(QString val);
    void addInfoPic(QImage img);
    void clientSearchedInfo();
    void initClientLookupInfo();
    void setSelectedClientInfo();
    QImage profilePic;

    //COLIN STUFF////
    void getProgramCodes();
    void bookingSetup();
    void populateBooking();
    void setBooking(int row);
    void populateConfirm();
    void popEditPage();
    void populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch);
    void handleNewPayment(int row);
    void updateCheque(int row);
    double calcRefund(QDate old, QDate n);
    bool checkNumber(QString num);
    bool updateBooking(Booking b);
    void popClientFromId(QString id);
    void getTransactionFromRow(int row);
    void popBookFromRow();


    //COLIN END//////

    /*==========================================================================
    DEV TESTING AUXILIARY FUNCTIONS
    ==========================================================================*/
    QString browse();


signals:
    void displayPic(QByteArray a);

private slots:
    //COLIN SLOTS ////////////////////////////////////////
    void on_editManagePayment_clicked();
    void on_editCost_textChanged(const QString &arg1);
    void on_editCancel_textChanged(const QString &arg1);
    void on_pushButton_bookRoom_clicked();

    void on_btn_payDelete_clicked();
    void on_bookingSearchButton_clicked();
    void on_makeBookingButton_clicked();
    void on_makeBookingButton_2_clicked();
    void on_monthCheck_stateChanged(int arg1);
    void on_pushButton_processPaymeent_clicked();
    void on_lunchCheck_clicked();
    void on_startDateEdit_dateChanged(const QDate &date);
    void on_wakeupCheck_clicked();
    void on_endDateEdit_dateChanged(const QDate &date);
    void on_monthCheck_clicked(bool checked);
    void on_paymentButton_2_clicked();
    void on_cbox_payDateRange_activated(int index);
    void popManagePayment();
    void on_editDate_dateChanged(const QDate &date);
    void on_btn_payOutstanding_clicked();
    void on_editUpdate_clicked();

    //END COLIN///////////////////////////////////////////

    /*==========================================================================
    DEV TESTING - slots
    ==========================================================================*/
    void on_actionDB_Connection_triggered();
    void on_actionTest_Query_triggered();
    void on_actionFile_Upload_triggered();
    void on_actionDownload_Latest_Upload_triggered();
    void on_actionPrint_Db_Connections_triggered();
    void on_actionUpload_Display_Picture_triggered();
    void on_actionDownload_Profile_Picture_triggered();
    void displayPicThread();
    void updateDailyReportStats(QList<int> list);

    /*==========================================================================
    DETECT WIDGET CHANGING SIGNAL
    ==========================================================================*/
    void initCurrentWidget(int idx);

    /*==========================================================================
    DASHBOARD - slots
    ==========================================================================*/
    void on_bookButton_clicked();
    void on_clientButton_clicked();
    void on_paymentButton_clicked();
    void on_adminButton_clicked();
    void on_editbookButton_clicked();
    void on_reportsButton_clicked();
    void on_caseButton_clicked();
    
    /*==========================================================================
    REPORTS -slots
    ==========================================================================*/
    void on_reportsDateSelectorGo_btn_clicked();
    void on_reportsSetCurrentDate_btn_clicked();

    
    
    void resizeEvent();


    void on_EditUserButton_clicked();

    void on_EditProgramButton_clicked();

    void on_actionMain_Menu_triggered();


    void on_pushButton_RegisterClient_clicked();

    void on_button_cancel_client_register_clicked();

    

    void on_pushButton_search_client_clicked();

    void selected_client_info(int nRow, int nCol);

    void on_button_register_client_clicked();

    /*==========================================================================
    CLIENT REGISTRATION FUNCTION
    ==========================================================================*/

    void getListRegisterFields(QStringList* fieldList);
    
    bool check_client_register_form();

    void clear_client_register_form();

    void read_curClient_Information(QString ClientId);

    void on_button_clear_client_regForm_clicked();

    void on_button_cl_takePic_clicked();

    void on_button_cl_delPic_clicked();

    void addPic(QImage pict);

    void defaultRegisterOptions();

    void searchTransaction(QString clientId);

    void displayTransaction(QSqlQuery results);

    /*========================================================================*/


    void on_btn_createNewUser_clicked();


    void on_editSearch_clicked();
    void on_btn_dailyReport_clicked();

    void on_btn_shiftReport_clicked();

    void on_btn_dailyLog_clicked();

    void on_btn_floatCount_clicked();

    void on_confirmationFinal_clicked();

    void on_editButton_clicked();
    void on_pushButton_editClientInfo_clicked();

    void on_btn_listAllUsers_clicked();

    void on_btn_searchUsers_clicked();

    void on_pushButton_CaseFiles_clicked();

    void on_EditRoomsButton_clicked();

    void on_EditShiftsButton_clicked();


    void on_editRoom_clicked();

    void on_pushButton_4_clicked();

    void on_btn_displayUser_clicked();

    void on_pushButton_6_clicked();

    void on_tableWidget_3_clicked(const QModelIndex &index);

    void on_tableWidget_3_doubleClicked(const QModelIndex &index);


    void on_btn_payListAllUsers_clicked();
    void on_btn_listAllUsers_3_clicked();

    void on_btn_listAllUsers_2_clicked();

    void on_btn_searchUsers_2_clicked();

    void on_pushButton_25_clicked();

    void on_tableWidget_2_clicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    /*==========================================================================
    CASE FILES FUNCTIONS
    ==========================================================================*/
    void on_btn_caseFilter_clicked();

    void populate_tw_caseFiles(QStringList filter = QStringList());

    void on_tw_caseFiles_cellDoubleClicked(int row, int column);

//    void on_tw_pcpRela_itemChanged(QTableWidgetItem *item);

    void initPcp();

    void on_btn_pcpRela_clicked();

    void on_btn_pcpEdu_clicked();

    void on_btn_pcpSub_clicked();

    void on_btn_pcpAcc_clicked();

    void on_btn_pcpLife_clicked();

    void on_btn_pcpMent_clicked();

    void on_btn_pcpPhy_clicked();

    void on_btn_pcpLeg_clicked();

    void on_btn_pcpAct_clicked();

    void on_btn_pcpTrad_clicked();

    void on_btn_pcpOther_clicked();

    void on_btn_pcpKey_clicked();

    void populateCaseFiles(QString type = "all", int tableId = -1);

    void insertPcp(QTableWidget *tw, QString type);

    void on_btn_createNewUser_2_clicked();

    void on_pushButton_24_clicked();

    void on_btn_monthlyReport_clicked();

    void on_btn_restrictedList_clicked();


//    void on_actionBack_triggered();

//    void on_actionForward_triggered();

//    void addHistory(int n);

//    void on_pushButton_processPaymeent_clicked();

//    void on_lunchCheck_clicked();

//    void on_startDateEdit_dateChanged(const QDate &date);

    void on_btn_pcpRelaSave_clicked();

    void on_addbedtoprogram_clicked();

    void on_removebedfromprogram_clicked();

    void on_availablebedslist_clicked(const QModelIndex &index);

    void on_assignedbedslist_clicked(const QModelIndex &index);

    void on_actionBack_triggered();

    void on_actionForward_triggered();

    void addHistory(int n);





    void on_btn_pcpEduSave_clicked();

    void on_btn_pcpSubSave_clicked();

    void on_btn_pcpAccoSave_clicked();

    void on_btn_pcpLifeSave_clicked();

    void on_btn_pcpMentSave_clicked();

    void on_btn_pcpPhySave_clicked();

    void on_btn_pcpLegSave_clicked();

    void on_btn_pcpActSave_2_clicked();

    void on_btn_pcpTradSave_clicked();

    void on_btn_pcpOtherSave_clicked();

    void on_btn_pcpKeySave_clicked();

    void on_actionPcptables_triggered();

    void on_btn_pcpRelaUndo_clicked();

    void on_btn_pcpEduUndo_clicked();

    void on_btn_pcpSubUndo_clicked();

    void on_btn_pcpAccoUndo_clicked();

    void on_btn_pcpLifeUndo_clicked();

    void on_btn_pcpMentUndo_clicked();

    void on_btn_pcpPhyUndo_2_clicked();

    void on_btn_pcpLegUndo_clicked();

    void on_btn_pcpActUndo_clicked();

    void on_btn_pcpTradUndo_clicked();

    void on_btn_pcpOtherUndo_clicked();

    void on_btn_pcpPplUndo_clicked();

    void on_btn_notesSave_clicked();

    void on_btn_notesUndo_clicked();

    void on_btn_searchUsers_3_clicked();


    void populate_modRoom_cboxes();

    void on_btn_modRoomBdlg_clicked();

    void on_btn_modRoomFloor_clicked();

    void on_btn_modRoomRoom_clicked();

    void on_btn_modRoomType_clicked();

    void on_cbox_roomLoc_currentTextChanged(const QString &arg1);

    void on_cbox_roomFloor_currentTextChanged(const QString &arg1);

    void on_cbox_roomRoom_currentTextChanged(const QString &arg1);

    void on_cbox_roomType_currentIndexChanged(int index);

    void on_cbox_roomType_currentTextChanged(const QString &arg1);

    void on_tabWidget_cl_info_currentChanged(int index);

private:

    Ui::MainWindow *ui;
    MainWindow * mw;
    Booking * curBook;
    transaction * trans;
    Client * curClient;
    QString curClientID;

    bool pic_available = true;
    bool table_available = true;

    bool caseWorkerUpdated = true;

    QMap <QString, int> caseWorkerList;    //caseworker list

    QDir dir;
    
    void setupReportsScreen();
    void updateReportTables(QDate date = QDate::currentDate());
    void getDailyReportStats(QDate date = QDate::currentDate());

    QModelIndex lastprogramclicked = QModelIndex();
    QModelIndex assignedBedIndex;
    QModelIndex availableBedIndex;

    int curmodifyingspace = NOT_SET;


};

#endif // MAINWINDOW_H
