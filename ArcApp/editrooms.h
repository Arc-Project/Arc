#ifndef EDITROOMS_H
#define EDITROOMS_H

#include <QDialog>
#include "booking.h"
#include "databasemanager.h"
#include <QTableWidgetItem>
#include "swapper.h"
namespace Ui {
class EditRooms;
}

class EditRooms : public QDialog
{
    Q_OBJECT

public:
    explicit EditRooms(QWidget *parent = 0);
    ~EditRooms();
    EditRooms(QWidget *parent, Booking * curBook);
    void searchAvailable(QString program);
    void populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch);
    void getProgramCodes(QString cur);
    bool setup;
private slots:
    void on_editProgram_currentIndexChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_editSwap_clicked();

private:
    bool swapping;
    Booking * curBook;
    Ui::EditRooms *ui;
};

#endif // EDITROOMS_H
