#ifndef MYMODEL_H
#define MYMODEL_H


#include <QAbstractTableModel>
#include <QString>
#include <Vector>
#include <qDebug>
#include "shared.h"

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent);
    MyModel(QObject *parent, QStringList* data, int rowCount, int colCount);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setData(QStringList* data, int rowCount, int colCount, int reportType);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setTitle(QStringList* title);
    QStringList* tableData = NULL;
    int rows = 0;
    int cols = 0;
private:
    QStringList* tableTitle = NULL;
    int type;
signals:
    void modelDataUpdated(int type, int cols);
};

#endif // MYMODEL_H
