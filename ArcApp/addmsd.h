#ifndef ADDMSD_H
#define ADDMSD_H
#include "transaction.h"
#include <QDialog>

namespace Ui {
class AddMSD;
}

class AddMSD : public QDialog
{
    Q_OBJECT

public:
    explicit AddMSD(QWidget *parent = 0);
    AddMSD(QWidget *parent, transaction * trans);
    ~AddMSD();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::AddMSD *ui;
    transaction * transac;
};

#endif // ADDMSD_H
