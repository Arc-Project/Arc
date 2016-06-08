#ifndef VALIDATE_H
#define VALIDATE_H

#include <QDialog>
#include <QString>
#include "databasemanager.h"
#include <QTableWidget>
#include <QMessageBox>
namespace Ui {
class Validate;
}

class Validate : public QDialog
{
    Q_OBJECT

public:
    explicit Validate(QWidget *parent = 0);
    Validate(QWidget * parent,  QString clientId, double curBal, double fixBal, QString employee, QString shift);
    ~Validate();
    void setupUi();
    void populateATable(QTableWidget * table, QStringList headers, QStringList items, QSqlQuery result, bool stretch, bool trans);
    bool doMessageBox(QString message);


private slots:
    void on_valFix_clicked();

private:
    Ui::Validate *ui;
    double curBal, fixBal;
    QString clientId, employee, shift;
};

#endif // VALIDATE_H
