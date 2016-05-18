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
#define FLOATCOUNT          2
#define MONTHLYREPORT       3
#define RESTRICTIONS        4
#define DEFAULTTAB          0

#define NOREGISTER          0
#define NEWCLIENT           1
#define EDITCLIENT          2

#define PERSIONACASEPLAN 	0
#define RUNNINGNOTE			1
#define BOOKINGHISTORY		2
#define TRANSACTIONHISTORY	3

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include "databasemanager.h"
#include "bookingmanager.h"
#include <QTableWidgetItem>
#include <QMap>
#include "loginprompt.h"
#include "booking.h"
#include "transaction.h"
#include "client.h"
#include "shared.h"
#include "casefile.h"
#include "Utility.h"
#include "mymodel.h"
#include "report.h"
#include "editrooms.h"
#include "addmsd.h"
#include "mycalendar.h"
#include <QPointer>

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
    void searchClientListThread();
    void setup_searchClientTable(QSqlQuery results);
    void displayClientInfoThread(QString val);
    void addInfoPic(QImage img);
    void clientSearchedInfo();
    void initClientLookupInfo();
    void setSelectedClientInfo();
    void initClBookHistoryTable();
    void initClTransactionTable();

    void getRegisterLogFields(QStringList* fieldList);
    void getCurrentClientId();   //get client id from client list table
    void getClientInfo();
    void statusColor();
    


    //COLIN STUFF////
    void getProgramCodes();
    void bookingSetup();
    void populateBooking();
    void setBooking(int row);
    void populateConfirm();
    void popEditPage();
    void populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch);
    void handleNewPayment(int row);
    void updateCheque(int row, QString chequeNo);
    double calcRefund(QDate old, QDate n);
    bool checkNumber(QString num);
    bool updateBooking(Booking b);
    void popClientFromId(QString id);
    void getTransactionFromRow(int row);
    void popBookFromRow();
    void clearTable(QTableWidget * table);
    void setBookSummary();
    bool doMessageBox(QString message);

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
    void on_editCost_textChanged();
    void on_editCancel_textChanged();
    void on_pushButton_bookRoom_clicked();

    void on_btn_payDelete_clicked();
    void on_bookingSearchButton_clicked();
    void on_makeBookingButton_clicked();
    void on_makeBookingButton_2_clicked();
//    void on_monthCheck_stateChanged(int arg1);
    void on_pushButton_processPaymeent_clicked();
    void on_lunchCheck_clicked();
    void on_startDateEdit_dateChanged();
    void on_wakeupCheck_clicked();
    void on_endDateEdit_dateChanged();
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
    REPORTS - slots
    ==========================================================================*/
    void on_dailyReportGo_btn_clicked();
    void on_dailyReportCurrent_btn_clicked();
    void updateDailyReportStats(QList<int> list);
    void on_shiftReportGo_btn_clicked();
    void on_shiftReportCurrent_btn_clicked();
    void updateShiftReportStats(QStringList list);
    void on_saveOther_btn_clicked();
    void on_other_lineEdit_textEdited(const QString &text);
    void updateCashFloat(QDate date, int shiftNo, QStringList list);
    void updateCashFloatLastEditedLabels(QString empName, 
        QString currentDateStr, QString currentTimeStr);
    void on_saveFloat_btn_clicked();
    void on_calculateTotal_btn_clicked();
    void on_cashFloatGo_btn_clicked();
    void on_cashFloatCurrent_btn_clicked();
    void on_monthlyReportGo_btn_clicked();
    void updateMonthlyReportUi(QStringList list);
    void on_restrictionRefresh_btn_clicked();
    void on_noDatabaseConnection(QSqlDatabase* database);
    
    void on_reconnectedToDatabase();
    
    void resizeEvent();



    void on_EditUserButton_clicked();

    void on_EditProgramButton_clicked();

    void on_actionMain_Menu_triggered();


    /*==========================================================================
    CLIENT LOOPUP WIDGET FUNCTIONS
    ==========================================================================*/

    void on_pushButton_RegisterClient_clicked();

    void on_pushButton_search_client_clicked();

    void on_checkBox_search_anonymous_clicked(bool checked);    //SEARCH ANONYMOUS

    void selected_client_info(int nRow, int nCol);

    void initClientLookupTable();                               //initialize anonymouse

    void searchTransaction(QString clientId);

    void displayTransaction(QSqlQuery results);

    void displayBookHistory(QSqlQuery results);

    void searchBookHistory(QString clientId);

    /*==========================================================================
    CLIENT REGISTRATION FUNCTION
    ==========================================================================*/

    void getListRegisterFields(QStringList* fieldList); //GET LIST TO UPDATE DB

    void on_button_register_client_clicked();           //BUTTON REGISTER OR EDIT INFO

    bool check_client_register_form();                  //CHECK IF IT IS EMPTY OR NOT

    void clear_client_register_form();

    void read_curClient_Information(QString ClientId);

    void on_button_clear_client_regForm_clicked();

    void on_button_cancel_client_register_clicked();

    void on_button_cl_takePic_clicked();

    void on_button_cl_delPic_clicked();

    void addPic(QImage pict);

    void defaultRegisterOptions();



    /*========================================================================*/


    void on_btn_createNewUser_clicked();


    void on_editSearch_clicked();
    void on_btn_dailyReport_clicked();

    void on_btn_shiftReport_clicked();

    void on_btn_floatCount_clicked();

    void on_confirmationFinal_clicked();

    void on_editButton_clicked();
    void on_pushButton_editClientInfo_clicked();

//-------------------------------------------------------------------

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

    void on_tableWidget_search_client_itemClicked();

    void on_programDropdown_currentIndexChanged();

    void on_confirmAddLunch_clicked();

    void on_confirmAddWake_clicked();

    void on_editLunches_clicked();

    void on_editWakeup_clicked();

    void on_actionQuit_triggered();

    void on_pushButton_cl_trans_more_clicked();

    void useProgressDialog(QString msg, QFuture<void> future);

    void on_tableWidget_5_clicked(const QModelIndex &index);

    void on_pushButton_cl_book_more_clicked();

    

    void on_lineEdit_search_clientName_returnPressed();

    void on_btn_createNewUser_3_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_actionExport_to_PDF_triggered();

    void on_btn_newTypeLoc_clicked();

    void on_btn_delTypeLoc_clicked();

    void on_editroommodifybox_clicked(const QModelIndex &index);

    void on_chk_filter_clicked();

    void on_btn_payNew_clicked();

    void on_actionLogout_triggered();

    void on_actionReconnect_to_Database_triggered();

    void setValue(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage);

    void printPCP(const int recNo, const QString paramName, QVariant &paramValue, const int reportPage);

    void on_comboBox_3_currentTextChanged(const QString &arg1);

    void on_btn_saveShift_clicked();

    void on_editProgramDrop_currentIndexChanged(const QString &arg1);

    void on_editRemoveCheque_clicked();
    void on_tabw_casefiles_currentChanged(int index);


private:

    Ui::MainWindow *ui;
    MainWindow * mw;
    Booking * curBook;
    bool setup;
    transaction * trans;
    Client * curClient;
    QString curClientID;
    bool editOverLap;
//    bool pic_available = true;
//    bool table_available = true;

    bool caseWorkerUpdated = true;
   QImage profilePic;
    QMap <QString, int> caseWorkerList;    //caseworker list

    QDir dir;
    
    /*==========================================================================
    REPORTS - private
    ==========================================================================*/
    void setupReportsScreen(); 
    void updateDailyReportTables(QDate date);
    void getDailyReportStats(QDate date);
    void updateShiftReportTables(QDate date, int shiftNo);
    void getShiftReportStats(QDate date, int shiftNo);
    void getCashFloat(QDate date, int shiftNo);
    void getMonthlyReport(int month, int year);
    void updateRestrictionTables();



    QModelIndex lastprogramclicked = QModelIndex();
    QModelIndex assignedBedIndex;
    QModelIndex availableBedIndex;

    int curmodifyingspace = NOT_SET;

    QFutureWatcher<void> futureWatcher;
};

#endif // MAINWINDOW_H
