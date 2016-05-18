#include "editrooms.h"
#include "ui_editrooms.h"
#include <QMessageBox>
EditRooms::EditRooms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditRooms)
{
    ui->setupUi(this);
}
EditRooms::EditRooms(QWidget *parent, Booking * curBook, QString empId, QString shift, Client * cli) :
    QDialog(parent),
    ui(new Ui::EditRooms)
{
    setup = true;
    this->curBook = curBook;
    ui->setupUi(this);
    curClient = cli;
    this->empId = empId;
    this->shift = shift;
    getProgramCodes(curBook->program);
    searchAvailable(curBook->program);
    setup = false;
    swapClient = new Client();
    swapBook = new Booking();
    setCurbook();

}
EditRooms::~EditRooms()
{
    delete ui;
}
void EditRooms::showSwap(){
    ui->label_2->setHidden(false);
    ui->label_3->setHidden(false);
    ui->label_4->setHidden(false);
    ui->label_5->setHidden(false);
    ui->label_6->setHidden(false);
    ui->label_7->setHidden(false);
    ui->label_8->setHidden(false);
    ui->label_9->setHidden(false);
    ui->label_10->setHidden(false);
    ui->label_11->setHidden(false);
    ui->label_12->setHidden(false);
    ui->label_13->setHidden(false);
    ui->label_14->setHidden(false);
    ui->label_15->setHidden(false);
    ui->label_16->setHidden(false);
    ui->label_17->setHidden(false);
    /*ui->curAdjust->setHidden(false);
    ui->curCost->setHidden(false);
    ui->curEnd->setHidden(false);
    ui->curFinal->setHidden(false);
    ui->curProgram->setHidden(false);
    ui->currentUse->setHidden(false);
    ui->curRoom->setHidden(false);
    ui->curStart->setHidden(false);
    */
    ui->swapAdjust->setHidden(false);
    ui->swapCost->setHidden(false);
    ui->swapEnd->setHidden(false);
    ui->swapFinal->setHidden(false);
    ui->swapProgram->setHidden(false);
    ui->swapRoom->setHidden(false);
    ui->swapStart->setHidden(false);
    ui->swapUser->setHidden(false);

}

void EditRooms::hideSwap(){
   //ui->label_2->setHidden(true);
    ui->label_3->setHidden(true);
    //ui->label_4->setHidden(true);
   // ui->label_5->setHidden(true);
    //ui->label_6->setHidden(true);
    //ui->label_7->setHidden(true);
    //ui->label_8->setHidden(true);
    ui->label_9->setHidden(true);
    ui->label_10->setHidden(true);
    ui->label_11->setHidden(true);
    ui->label_12->setHidden(true);
    //ui->label_13->setHidden(true);
    ui->label_14->setHidden(true);
    ui->label_15->setHidden(true);
   // ui->label_16->setHidden(true);
    ui->label_17->setHidden(true);
    /*ui->curAdjust->setHidden(true);
    ui->curCost->setHidden(true);
    ui->curEnd->setHidden(true);
    ui->curFinal->setHidden(true);
    ui->curProgram->setHidden(true);
    ui->currentUse->setHidden(true);
    ui->curRoom->setHidden(true);
    ui->curStart->setHidden(true);
    */
    ui->swapAdjust->setHidden(true);
    ui->swapCost->setHidden(true);
    ui->swapEnd->setHidden(true);
    ui->swapFinal->setHidden(true);
    ui->swapProgram->setHidden(true);
    ui->swapRoom->setHidden(true);
    ui->swapStart->setHidden(true);
    ui->swapUser->setHidden(true);


}

void EditRooms::searchAvailable(QString program){
    swapping = false;
    addQuery = false;
    hideSwap();
    ui->editMoveType->setText("Move to Free Room");

    QSqlQuery result;
    result = dbManager->getCurrentBooking(curBook->startDate, curBook->endDate, program);
    QStringList headers;
    QStringList col;
    headers << "Room#" <<  "Program" << "Type" << "Cost" << "Monthly" << "";
    col << "SpaceCode" << "ProgramCodes" << "type" << "cost" << "Monthly" << "SpaceId";

    populateATable(ui->editRoom,headers, col, result, false);
    ui->editRoom->setColumnHidden(5, true);
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
    fastQuery.clear();
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
            QString add = result.value(items.at(i)).toString();
            if(addQuery)
                fastQuery << add;
            table->setItem(x, i, new QTableWidgetItem(add));


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
void EditRooms::updateClient(int row){
    curBook->cost = ui->curFinal->text().toDouble();
    curBook->room = ui->editRoom->item(row, 0)->text();
    curBook->roomId = ui->editRoom->item(row,5)->text();
    curBook->program = ui->editProgram->currentText();
}

void EditRooms::on_buttonBox_accepted()
{

}

void EditRooms::on_editSwap_clicked()
{
    showSwap();
    addQuery = true;
    ui->editMoveType->setText("Moving to Occupied Room");
    QSqlQuery result;
    result = dbManager->getNextBooking(curBook->endDate, curBook->roomId);
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
    headers << "Room#" <<  "Program" << "Type" << "Cost" << "Monthly" << "Current Occupant" << "" << "" << "Client Name" << "" << "" << "" << "";
    col << "SpaceCode" << "ProgramCodes" << "type" << "cost" << "Monthly" << "ClientName" << "BookingId" << "SpaceId" << "ClientId" <<
         "SpaceId" << "StartDate" << "EndDate" << "Cost";
    populateATable(ui->editRoom,headers, col, result, false);
    ui->editRoom->setColumnHidden(6, true);
    ui->editRoom->setColumnHidden(7, true);
    ui->editRoom->setColumnHidden(9, true);
    ui->editRoom->setColumnHidden(10, true);
    ui->editRoom->setColumnHidden(11, true);
    ui->editRoom->setColumnHidden(12, true);
    swapping = true;
    setCurbook();

}
void EditRooms::setCurbook(){
    ui->curCost->setText(QString::number(curBook->cost,'f',2));
    ui->curEnd->setText(curBook->stringEnd);
    ui->curStart->setText(curBook->stringStart);
    ui->curProgram->setText(curBook->program);
    ui->curRoom->setText(curBook->room);
    ui->currentUse->setText(curClient->fullName);

}

void EditRooms::on_editRoom_itemSelectionChanged()
{
    int row = ui->editRoom->selectionModel()->currentIndex().row();
    if(row == -1)
        return;

    if(!swapping){
        setNewPrice(row);

    }
    else{

        popSwapClient(row);
        displaySwapClient();
    }
}
bool EditRooms::checkNumber(QString num){
    int l = num.length();
    int period = 0;
    if(l > 8)
        return false;
    char copy[l];
    strcpy(copy, num.toStdString().c_str());
    for(int i = 0; i < num.length(); i++){
        if(copy[i] == '.'){
            if(period)
                return false;
            period++;
            continue;
        }
        /*if(copy[i] == '-'){
            if(i == 0)
                continue;
             return false;
        }
*/
        if(!isdigit(copy[i]))
            return false;
    }
    return true;
}
void EditRooms::popSwapClient(int row){
    int startPoint = row * 13;
    swapBook->bookID = fastQuery.at(startPoint + 6);
    swapBook->clientId = fastQuery.at(startPoint + 8);
    QString cost = fastQuery.at(startPoint +  12);
    swapBook->cost =  cost.toDouble();
    cost = fastQuery.at(startPoint + 3);
    swapBook->costDaily = cost.toDouble();
    swapBook->endDate = QDate::fromString(fastQuery.at(startPoint + 11), "yyyy-MM-dd");
    swapBook->startDate  = QDate::fromString(fastQuery.at(startPoint + 10),"yyyy-MM-dd");
    swapBook->roomId =  fastQuery.at(startPoint + 9);
    swapBook->room = fastQuery.at(startPoint);
    swapBook->stayLength = swapBook->endDate.toJulianDay() - swapBook->startDate.toJulianDay();
    swapBook->program = fastQuery.at(startPoint + 1);
    swapClient->clientId = fastQuery.at(startPoint + 8);
    swapClient->fullName = fastQuery.at(startPoint + 5);
    swapBook->costMonthly = fastQuery.at(startPoint + 4).toDouble();
    double posCost = fastQuery.at(startPoint + 4).toDouble();
    if(swapBook->cost == posCost){
        swapBook->monthly = true;
    }
    else
        swapBook->monthly = false;

}
void EditRooms::setNewPrice(int row){
    double cost = ui->editRoom->item(row, 3)->text().toDouble();
    double monthly = ui->editRoom->item(row, 4)->text().toDouble();
    ui->curProgram->setText(ui->editRoom->item(row,1 )->text());
    cost *= curBook->stayLength;
    if(!curBook->monthly){
        ui->curAdjust->setText(QString::number(cost, 'f' ,2));
        ui->curFinal->setText(QString::number(cost, 'f', 2));
    }
    else{
        ui->curFinal->setText(QString::number(monthly, 'f', 2));
        ui->curAdjust->setText(QString::number(monthly, 'f', 2));
    }
}

void EditRooms::displaySwapClient(){
    ui->swapEnd->setText(swapBook->endDate.toString(Qt::ISODate));
    ui->swapStart->setText(swapBook->startDate.toString(Qt::ISODate));
    ui->swapProgram->setText(swapBook->program);
    ui->swapRoom->setText(swapBook->room);
    ui->swapUser->setText(swapClient->fullName);
    calcCosts();
    ui->swapCost->setText(QString::number(swapBook->cost));


}
void EditRooms::calcCosts(){
    double curCost, swapCost;
    if(curBook->monthly)
        curCost = swapBook->costMonthly;
    else
        curCost = swapBook->costDaily * curBook->stayLength;

    QSqlQuery result;
    result = dbManager->getRoomCosts(curBook->roomId);
    result.next();
    double cost = result.value("cost").toString().toDouble();
    result = dbManager->getBooking(swapBook->bookID);
    if(!result.next())
        qDebug() << "Error getting swap booking";
    QString m = result.value("Monthly").toString();
    double monthly = m.toDouble();
    QString c = result.value("Cost").toString();
    swapBook->cost = c.toDouble();
    m == "YES" ? swapBook->monthly = true : swapBook->monthly = false;
    if(swapBook->monthly)
        swapCost = monthly;
    else{
        swapCost = swapBook->stayLength * cost;
    }

    ui->swapAdjust->setText(QString::number(swapCost, 'f', 2));
    ui->curAdjust->setText(QString::number(curCost, 'f', 2));
    ui->swapFinal->setText(QString::number(swapCost,'f', 2));
    ui->curFinal->setText(QString::number(curCost, 'f', 2));
}
bool EditRooms::doMessageBox(QString message){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm", message, QMessageBox::Yes | QMessageBox::No);
    if(reply == QMessageBox::Yes){
        return true;
    }
    return false;

}

void EditRooms::on_editOkButton_clicked()
{
    if(ui->editRoom->selectionModel()->currentIndex().row() == -1)
        return;

    int row;
    if(!doMessageBox("Room change and cost are final. Continue?"))
        return;
    row = ui->editRoom->selectionModel()->currentIndex().row();
    if(!swapping){
        if(!checkNumber(ui->curFinal->text())){
            return;
        }
        updateClient(row);
        double costDiff;
        costDiff = ui->curCost->text().toDouble() - curBook->cost;
        dbManager->updateBalance(curClient->balance + costDiff, curClient->clientId);
        QString q = "UPDATE Booking SET Cost='" + QString::number(curBook->cost) +"', SpaceId ='" + curBook->roomId +
                "' WHERE BookingId='" + curBook->bookID + "'";
        dbManager->updateBooking(q);

        dbManager->addHistoryFromId(curBook->bookID, empId, shift, "EDIT");

    }
    else{

        double curBalance, swapBalance;
        double curDiff, swapDiff;
        QString swapBooks = ui->editRoom->item(row, 6)->text();
        QString swapRoom = ui->editRoom->item(row, 7)->text();
        QString curCost, swapCost;
        curCost = ui->curFinal->text();
        swapCost = ui->swapFinal->text();
        if(!checkNumber(curCost) || !checkNumber(swapCost))
            return;
        QString updateSwap = "UPDATE Booking SET SpaceId = '" + curBook->roomId +"', Cost ='" + curCost + "' WHERE BookingId = '" + swapBooks +"'";
        if(dbManager->updateBooking(updateSwap)){
            QString updateOrig = "UPDATE Booking SET SpaceId = '" + swapRoom +"', Cost='" + swapCost + "' WHERE BookingId = '" + curBook->bookID + "'";
            if(dbManager->updateBooking(updateOrig)){
                curBook->roomId = swapRoom;
            if(!dbManager->updateWakeupRoom(QDate::currentDate(), swapBook->endDate, swapClient->clientId, curBook->room))
                qDebug() << "Error with update wakeup";
            if(!dbManager->updateLunchRoom(QDate::currentDate(), swapBook->endDate, swapClient->clientId, curBook->room))
                qDebug() << "Error with update lunch";

                QSqlQuery result;
                result = dbManager->getBalance(curClient->clientId);
                result.next();
                curBalance = result.value("Balance").toString().toDouble();
                result = dbManager->getBalance(swapClient->clientId);
                result.next();
                swapBalance = result.value("Balance").toString().toDouble();
                QString fqt = ui->curFinal->text();
                curDiff = curBook->cost - fqt.toDouble();
                fqt = ui->swapFinal->text();
                swapDiff = swapBook->cost - fqt.toDouble();
                curClient->balance = curBalance + curDiff;
                swapClient->balance = swapBalance + swapDiff;
                curBook->cost = curCost.toDouble();
                curBook->room = swapBook->room;
                curBook->roomId = swapBook->roomId;
                if(swapDiff != 0){

                    dbManager->updateBalance(swapClient->balance, swapClient->clientId);
                }
                if(curDiff != 0){
                    dbManager->updateBalance(curClient->balance, curClient->clientId);
                }

            }
            else{
                qDebug() << "ERROR INSERTING SECOND SWAP";
            }
        }
        else{
            qDebug() << "ERROR INSERTING SWAP";
        }
        dbManager->addHistoryFromId(swapBooks, empId, shift, "SWAP");
        dbManager->addHistoryFromId(curBook->bookID, empId, shift, "SWAP");

    }
    if(!dbManager->updateLunchRoom(QDate::currentDate(), curBook->endDate, curClient->clientId, curBook->room))
        qDebug() << "Error updating lnch room";
    if(!dbManager->updateWakeupRoom(QDate::currentDate(), curBook->endDate, curClient->clientId, curBook->room))
        qDebug() << "Error updating wkeup room";
    close();

}

void EditRooms::on_editCancelButton_clicked()
{
    close();
}
