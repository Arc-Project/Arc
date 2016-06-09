#ifndef DUPLICATECLIENTS_H
#define DUPLICATECLIENTS_H
#include <QSqlQuery>
#include <QDialog>

namespace Ui {
class DuplicateClients;
}

class DuplicateClients : public QDialog
{
    Q_OBJECT

public:
    explicit DuplicateClients(QWidget *parent = 0);
    ~DuplicateClients();
    void displayList(QSqlQuery info);

private slots:

    void on_pushButton_cl_warning_Select_clicked();

    void on_tableWidget_same_name_cellClicked(int row, int column);

    void on_pushButton_cl_warning_ignore_clicked();

signals:
    void selectedUser(QString clientID);
    void ignoreWarning();

private:
    Ui::DuplicateClients *ui;
    int nRow = -1;
};

#endif // DUPLICATECLIENTS_H
