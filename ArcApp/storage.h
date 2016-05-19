#ifndef STORAGE_H
#define STORAGE_H

#include <QDialog>
#include "databasemanager.h"


namespace Ui {
class Storage;
}

class Storage : public QDialog
{
    Q_OBJECT

public:
    explicit Storage(QWidget *parent = 0);
    Storage(QWidget *parent, QString clientId, QString clientName, QString prev, QString storageId);
    ~Storage();
    void loadPrevious();
    bool doMessageBox(QString message);

private slots:
    void on_storageOk_clicked();

    void on_storageCancel_clicked();

private:
    Ui::Storage *ui;
    QString id, name;
    QString storeId;
    bool newStore;

};

#endif // STORAGE_H
