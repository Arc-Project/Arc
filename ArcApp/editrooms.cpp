#include "editrooms.h"
#include "ui_editrooms.h"

EditRooms::EditRooms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditRooms)
{
    ui->setupUi(this);
}
EditRooms::EditRooms(QWidget *parent, Booking * curBook) :
    QDialog(parent),
    ui(new Ui::EditRooms)
{
    setup = true;
    this->curBook = curBook;
    ui->setupUi(this);
    getProgramCodes(curBook->program);
    searchAvailable(curBook->program);
    setup = false;

}
EditRooms::~EditRooms()
{
    delete ui;
}
void EditRooms::searchAvailable(QString program){
    swapping = false;
    QSqlQuery result;
    result = dbManager->getCurrentBooking(curBook->startDate, curBook->endDate, program);
    QStringList headers;
    QStringList col;
    headers << "Room#" <<  "Program" << "Type" << "Cost" << "Monthly";
    col << "SpaceId" << "ProgramCodes" << "type" << "cost" << "Monthly";
    populateATable(ui->editRoom,headers, col, result, false);

}

void EditRooms::getProgramCodes(QString cur){
    QSqlQuery result = dbManager->getPrograms();
//    int i = 0;
    ui->editProgram->clear();
    int x = 0;
    while(result.next()){
        QString add = result.value(0).toString();
        ui->editProgram->addItem(add);
        if(add == cur){
            ui->editProgram->setCurrentIndex(x);
        }
        x++;
    }
}
void EditRooms::populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch){
    table->clear();
    table->setRowCount(0);

    if(headers.length() != items.length())
        return;

    if(stretch)
        table->horizontalHeader()->setStretchLastSection(true);
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


        x++;
    }
}
void EditRooms::on_editProgram_currentIndexChanged(const QString &arg1)
{
    if(setup)
        return;
    searchAvailable(arg1);
}

void EditRooms::on_buttonBox_accepted()
{
    if(ui->editRoom->selectionModel()->currentIndex().row() == -1)
        return;

    int row;
    row = ui->editRoom->selectionModel()->currentIndex().row();
    if(!swapping){
       if(curBook->monthly){
            curBook->cost = ui->editRoom->item(row,4)->text().toDouble();
        }
        else{

            curBook->cost = ui->editRoom->item(row,3 )->text().toDouble() * curBook->stayLength;
        }
        curBook->room = ui->editRoom->item(row,0)->text();
        curBook->program = ui->editRoom->item(row,1)->text();
    }
    else{
        QString swapBook = ui->editRoom->item(row, 6)->text();

        QString swapRoom = ui->editRoom->item(row, 0)->text();

        QString updateSwap = "UPDATE Booking SET SpaceId = '" + curBook->room +"' WHERE BookingId = '" + swapBook +"'";
        if(dbManager->updateBooking(updateSwap)){
            QString updateOrig = "UPDATE Booking SET SpaceId = '" + swapRoom +"' WHERE BookingId = '" + curBook->bookID + "'";
            if(dbManager->updateBooking(updateOrig)){
                curBook->room = swapRoom;
            }
            else{
                qDebug() << "ERROR INSERTING SECOND SWAP";
            }
        }
        else{
            qDebug() << "ERROR INSERTING SWAP";
        }

    }


}

void EditRooms::on_editSwap_clicked()
{
    swapping = true;
    QSqlQuery result;
    result = dbManager->getNextBooking(curBook->endDate, curBook->room);
    int x = 0;
    QDate nextStart;
    while(result.next()){
        if(x == 0){
            nextStart = QDate::fromString(result.value("StartDate").toString(), "yyyy-MM-dd");
        }
        x++;
    }
    if(!x){
        nextStart = QDate::fromString("2500-05-05", "yyyy-MM-dd");
    }

    result = dbManager->getSwapBookings(curBook->startDate, curBook->endDate, curBook->program, nextStart.toString(Qt::ISODate), curBook->bookID);
    QStringList headers;
    QStringList col;
    headers << "Room#" <<  "Program" << "Type" << "Cost" << "Monthly" << "Current Occupant" << "";
    col << "SpaceId" << "ProgramCodes" << "type" << "cost" << "Monthly" << "ClientName" << "BookingId" ;
    ui->editRoom->setColumnHidden(6, true);
    populateATable(ui->editRoom,headers, col, result, false);
}
