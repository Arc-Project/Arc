#include "validate.h"
#include "ui_validate.h"

Validate::Validate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Validate)
{
    ui->setupUi(this);
}
Validate::Validate(QWidget *parent, QString clientId, double curBal, double fixBal, QString employee, QString shift) :
    QDialog(parent),
    ui(new Ui::Validate)
{
    this->employee = employee;
    this->shift = shift;
    this->clientId = clientId;
    this->curBal = curBal;
    this->fixBal = fixBal;
    ui->setupUi(this);
    setupUi();
}

Validate::~Validate()
{
    delete ui;
}
void Validate::setupUi(){
    QSqlQuery result;
    result = dbManager->getClientBooking(clientId);
    QStringList headers,cols;
    headers << "Start" << "Checkout" << "Cost";
    cols << "StartDate" << "EndDate" << "Cost";
    populateATable(ui->valBook, headers,cols, result, false, false);
    result = dbManager->getClientTransactions(clientId);
    headers.clear();
    cols.clear();
    headers << "Date" << "Type" << "Notes" << "Amount";
    cols << "Date" << "TransType" << "Notes" << "Amount";
    populateATable(ui->valTrans, headers, cols, result, false, true);
    ui->valCur->setText(QString::number(curBal, 'f',2));
    ui->valExpect->setText(QString::number(fixBal, 'f', 2));
}
bool Validate::doMessageBox(QString message){
    QString tmpStyleSheet=parentWidget()->styleSheet();
    parentWidget()->setStyleSheet("");

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", message, QMessageBox::Yes | QMessageBox::No);
    
    parentWidget()->setStyleSheet(tmpStyleSheet);
    
    if(reply == QMessageBox::Yes){
        return true;
    }
    return false;

}
void Validate::on_valFix_clicked()
{
    if(!doMessageBox("Set clients balance to expected?"))
        return;
    double dif = fixBal - curBal;
    QString type = "Payment";
    if(dif > 0){
        type = "Refund";
    }
    else{
        dif *= -1;
    }



    if(!dbManager->updateBalance(fixBal, clientId)){
        return;
    }



    //if(!dbManager->escapePayment(clientId, QDate::currentDate().toString(Qt::ISODate), QString::number(dif, 'f', 2), "FIX", "Fixing Balance", "", "", "", type, "", employee, shift, QTime::currentTime().toString()))
      //  return;
    close();
}
void Validate::populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch, bool trans){

    table->clear();
    table->setRowCount(0);
    if(headers.length() != items.length())
        return;

    //if(stretch)
       // table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->hide();
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    int colCount = headers.size();
    table->setColumnCount(colCount);
    if(headers.length() != 0){
        table->setHorizontalHeaderLabels(headers);
    }
    int x = 0;
    while(result.next()){
        table->insertRow(x);
        for(int i = 0; i < colCount; i++){
            table->setItem(x, i, new QTableWidgetItem(result.value(items.at(i)).toString()));



        }
        if(trans){
            QString type = result.value("TransType").toString();
            if(type == "Payment"){
                table->item(x, 3)->setBackgroundColor(Qt::green);

            }
            else{
                table->item(x, 3)->setBackgroundColor(Qt::red);
            }
        }

        x++;
    }


}

void Validate::resizeTableView(QTableView* tableView)
{
    tableView->setVisible(false);
    tableView->resizeColumnsToContents();

    int cols = tableView->horizontalHeader()->count();
    double width = tableView->horizontalHeader()->size().width();
    double currentWidth = 0;
    int hiddenColCount = 0;
    for (int i = 0; i < cols; ++i)
    {
        double colWidth = tableView->columnWidth(i);
        if (colWidth == 0)
        {
            hiddenColCount++;
        }
        currentWidth +=  colWidth;
    }
    double diff = width - currentWidth;
    if (diff > 0)
    {
        double sizeIncrease = diff / (cols - hiddenColCount);

        for (int i = 0; i < cols; ++i)
        {
            if (diff > currentWidth * 1.5f)
            {
                sizeIncrease = tableView->columnWidth(i) * 0.75f;
            }
            if (tableView->columnWidth(i) != 0)
            {
                tableView->setColumnWidth(i, tableView->columnWidth(i) + sizeIncrease);
            }
        }
    }

    tableView->setVisible(true);
}

void Validate::on_valExit_clicked()
{
    close();
}

void Validate::showEvent( QShowEvent* event ) {
    QWidget::showEvent( event );
    resizeTableView(ui->valTrans);
    resizeTableView(ui->valBook);
} 
