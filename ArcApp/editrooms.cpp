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
    setDailyCost();

}
void EditRooms::setDailyCost(){
    QSqlQuery result = dbManager->getRoomCosts(curBook->roomId);
    if(!result.next()){
        doMessageBox("The current room is not found, all cost calculations will be invalid");
        curBook->costDaily = 0;
        curBook->costMonthly = 0;
    }
    curBook->costDaily = result.value("cost").toString().toDouble();
    curBook->costMonthly = result.value("Monthly").toString().toDouble();

    curClient->balance = dbManager->getDoubleBalance(curBook->clientId);
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
std::pair<int,int> EditRooms::monthDay(QDate start, QDate end){
    int days, months, totalDays;
    months = 0;
    totalDays = end.toJulianDay() - start.toJulianDay();
    QDate holdEnd;
    holdEnd = end;
    while((holdEnd = holdEnd.addMonths(-1)) >= start)
        months++;
    holdEnd = holdEnd.addMonths(1);
    if(holdEnd == start)
        days = 0;
    else{
        holdEnd = start.addMonths(months);
        days = end.toJulianDay() - holdEnd.toJulianDay();
    }
    std::pair<int,int> p = std::make_pair(months, days);
    return p;
}


void EditRooms::searchAvailable(QString program){
    swapping = false;
    addQuery = false;
    hideSwap();
    ui->editMoveType->setText("Moving current client to an unoccupied room");

    QSqlQuery result;
    result = dbManager->getCurrentBooking(curBook->startDate, curBook->endDate, program);
    QStringList headers;
    QStringList col;
    headers << "Space #" <<  "Program" << "Type" << "Daily" << "Monthly" << "";
    col << "SpaceCode" << "ProgramCodes" << "type" << "cost" << "Monthly" << "SpaceId";

    populateATable(ui->editRoom,headers, col, result, false);
    ui->editRoom->setColumnHidden(5, true);

    MainWindow::addCurrencyNoSignToTableWidget(ui->editRoom, 3);
    MainWindow::addCurrencyNoSignToTableWidget(ui->editRoom, 4);
    MainWindow::resizeTableView(ui->editRoom);
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
    ui->editMoveType->setText("Moving current client to an occupied room");
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
    headers << "Space #" <<  "Program" << "Type" << "Daily" << "Monthly" << "Current Occupant" << "" << "" << "Client Name" << "" << "" << "" << "";
    col << "SpaceCode" << "ProgramCodes" << "type" << "cost" << "Monthly" << "ClientName" << "BookingId" << "SpaceId" << "ClientId" <<
         "SpaceId" << "StartDate" << "EndDate" << "Cost";
    populateATable(ui->editRoom,headers, col, result, false);
    // ui->editRoom->setColumnWidth(5, 300);
    ui->editRoom->setColumnHidden(8, true);
    ui->editRoom->setColumnHidden(6, true);
    ui->editRoom->setColumnHidden(5, false);
    ui->editRoom->setColumnHidden(7, true);
    ui->editRoom->setColumnHidden(9, true);
    ui->editRoom->setColumnHidden(10, true);
    ui->editRoom->setColumnHidden(11, true);
    ui->editRoom->setColumnHidden(12, true);
    swapping = true;
    setCurbook();

    MainWindow::addCurrencyNoSignToTableWidget(ui->editRoom, 3);
    MainWindow::addCurrencyNoSignToTableWidget(ui->editRoom, 4);
    MainWindow::resizeTableView(ui->editRoom);
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
    swapBook->cost = dbManager->getBookingCost(swapBook->bookID);


}
void EditRooms::setNewPrice(int row){
    double cost = ui->editRoom->item(row, 3)->text().toDouble();
    double monthly = ui->editRoom->item(row, 4)->text().toDouble();
    double curCost;
    double curExpected;
    std::pair<int,int> curStay;
    double dayCost, monthCost;
    double curDiscount =1;
    bool curDisc = false;
    curStay = monthDay(curBook->startDate, curBook->endDate);
    dayCost = curBook->costDaily * curStay.second;
    if(dayCost > curBook->costMonthly)
        dayCost = curBook->costMonthly;
    monthCost = curBook->costMonthly * curStay.first;
    curExpected = dayCost + monthCost;
    if(curBook->cost != curExpected){
        ui->editWarnLabel->setText("WARNING: Booking includes discount or non standard rate, check pricing calculations");
        ui->editWarnLabel->setHidden(false);
        if(curBook->cost == 0){
            curDiscount = 0;
        }
        else if(curExpected == 0){
            curDiscount = 1;

        }
        else{
            curDiscount = curBook->cost / curExpected;
        }
        curDisc = true;

    }
    else{
        ui->editWarnLabel->setHidden(true);
    }
    Booking temp;
    temp.costMonthly = monthly;
    temp.costDaily = cost;
    curCost = getRealCost(curBook, &temp, curDisc, curDiscount);
    ui->curFinal->setText(QString::number(curCost,'f',2));
    ui->curAdjust->setText(QString::number(curCost, 'f',2));

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
    double curExpected, swapExpected;
    std::pair<int,int> curStay;
    std::pair<int,int> swapStay;
    double dayCost, monthCost;
    double curDiscount, swapDiscount;
    double curAlready, swapAlready;
    bool curDisc = false;
    curDiscount = 1;
    swapDiscount = 1;
    bool swapDisc = false;
    curStay = monthDay(curBook->startDate, curBook->endDate);
    swapStay = monthDay(swapBook->startDate, swapBook->endDate);
    dayCost = curBook->costDaily * curStay.second;
    if(dayCost > curBook->costMonthly)
        dayCost = curBook->costMonthly;
    monthCost = curBook->costMonthly * curStay.first;
    curExpected = dayCost + monthCost;
    dayCost = swapBook->costDaily * swapStay.second;
    if(dayCost > swapBook->costMonthly)
        dayCost = swapBook->costMonthly;
    monthCost = swapBook->costMonthly * swapStay.first;
    swapExpected = dayCost + monthCost;
    if(curBook->cost != curExpected){
        ui->editWarnLabel->setText("WARNING: Booking includes discount or non standard rate, check pricing calculations");
        ui->editWarnLabel->setHidden(false);
        if(curBook->cost == 0){
            curDiscount = 0;
        }
        else if(curExpected == 0){
            curDiscount = 1;

        }
        else{
            curDiscount = curBook->cost / curExpected;
            if(curDiscount > 1)
                curDiscount = 1;
        }
        curDisc = true;

    }
    if(swapBook->cost != swapExpected){
        ui->editWarnLabel->setText("WARNING: Booking includes discount or non standard rate, check pricing calculations");
        ui->editWarnLabel->setHidden(false);
        if(swapBook->cost == 0){
            swapDiscount = 0;
        }
        else if(swapExpected == 0){
            swapDiscount = 1;

        }
        else{
            swapDiscount = swapBook->cost / swapExpected;
            if(swapDiscount > 1)
                swapDiscount = 1;
        }
        swapDisc = true;
    }
    if(swapBook->cost == swapExpected && curBook->cost == curExpected){
        ui->editWarnLabel->setHidden(true);
    }
    curCost = getRealCost(curBook, swapBook, curDisc, curDiscount);
    swapCost = getRealCost(swapBook, curBook, swapDisc, swapDiscount);



    ui->swapAdjust->setText(QString::number(swapCost, 'f', 2));
    ui->curAdjust->setText(QString::number(curCost, 'f', 2));
    ui->swapFinal->setText(QString::number(swapCost,'f', 2));
    ui->curFinal->setText(QString::number(curCost, 'f', 2));
}
double EditRooms::getRealCost(Booking * cur, Booking * swap, bool discount, double discAmt){
    double timeSpent;
    double timePerc, invPerc;
    double dailyDiscCost;
    double totalCost;
    double retCost;
    double stayLen = cur->endDate.toJulianDay() - cur->startDate.toJulianDay();
    if(cur->startDate > QDate::currentDate()){
        timeSpent = 0;
    }else{
        timeSpent = QDate::currentDate().toJulianDay() - cur->startDate.toJulianDay();
        if(cur->endDate == QDate::currentDate())
            timeSpent--;

    }
    timePerc = timeSpent / stayLen;
    invPerc = 1 - timePerc;
    //if(discount){
      //  dailyDiscCost = discAmt * swap->costDaily;
       // totalCost = dailyDiscCost * stayLen;
       // totalCost *= invPerc;
        //totalCost += cur->cost * timePerc;

    //}
   // else{
        std::pair<int,int> stayStuff = monthDay(cur->startDate, cur->endDate);
        totalCost = quickCost(stayStuff, swap->costDaily, swap->costMonthly);
        totalCost *= invPerc * discAmt;
        totalCost += timePerc * cur->cost;

    //}
    return totalCost;

}
double EditRooms::quickCost(std::pair<int,int> p, double daily, double monthly){
    int days, months;
    double dailyCost, totalCost;
    days = p.second;
    months = p.first;
    dailyCost = days * daily;
    if(dailyCost > monthly){
        dailyCost = monthly;
    }

    totalCost = monthly * months + dailyCost;
    return totalCost;


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
        curClient->balance+= costDiff;
        QStringList spaceInfo = dbManager->getSpaceInfoFromId(curBook->roomId.toInt());
        QString addSpaceInfoString = "";
        if (spaceInfo.isEmpty())
        {
            qDebug() << "Empty spaceInfo list";
        }
        else
        {
            addSpaceInfoString = QString("BuildingNo = '" + spaceInfo.at(0) + "', ")    
                + QString("FloorNo = '" + spaceInfo.at(1) + "', ")
                + QString("RoomNo = '" + spaceInfo.at(2) + "', ")
                + QString("SpaceNo = '" + spaceInfo.at(3) + "', ")
                + QString("SpaceCode = '" + spaceInfo.at(4) + "', ");
        }


        QString q = "UPDATE Booking SET Cost='" + QString::number(curBook->cost) +"', " + addSpaceInfoString + "SpaceId ='" + curBook->roomId +
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

        QStringList spaceInfoSwap = dbManager->getSpaceInfoFromId(curBook->roomId.toInt());
        QString addSpaceInfoStringSwap = "";
        if (spaceInfoSwap.isEmpty())
        {
            qDebug() << "Empty spaceInfo list";
        }
        else
        {
            addSpaceInfoStringSwap = QString("BuildingNo = '" + spaceInfoSwap.at(0) + "', ")
                + QString("FloorNo = '" + spaceInfoSwap.at(1) + "', ")
                + QString("RoomNo = '" + spaceInfoSwap.at(2) + "', ")
                + QString("SpaceNo = '" + spaceInfoSwap.at(3) + "', ")
                + QString("SpaceCode = '" + spaceInfoSwap.at(4) + "', ");
        }

        QStringList spaceInfoOrig = dbManager->getSpaceInfoFromId(swapRoom.toInt());
        QString addSpaceInfoStringOrig = "";
        if (spaceInfoOrig.isEmpty())
        {
            qDebug() << "Empty spaceInfo list";
        }
        else
        {
            addSpaceInfoStringOrig = QString("BuildingNo = '" + spaceInfoOrig.at(0) + "', ")
                + QString("FloorNo = '" + spaceInfoOrig.at(1) + "', ")
                + QString("RoomNo = '" + spaceInfoOrig.at(2) + "', ")
                + QString("SpaceNo = '" + spaceInfoOrig.at(3) + "', ")
                + QString("SpaceCode = '" + spaceInfoOrig.at(4) + "', ");
        }

        QString updateSwap = "UPDATE Booking SET SpaceId = '" + curBook->roomId +"', " + addSpaceInfoStringSwap + "Cost ='" + curCost + "' WHERE BookingId = '" + swapBooks +"'";
        if(dbManager->updateBooking(updateSwap)){
            QString updateOrig = "UPDATE Booking SET SpaceId = '" + swapRoom +"', " + addSpaceInfoStringOrig + "Cost='" + swapCost + "' WHERE BookingId = '" + curBook->bookID + "'";
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

