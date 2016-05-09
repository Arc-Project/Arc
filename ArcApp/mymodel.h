#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <Vector>

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent, QStringList* data, int rowCount, int colCount);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    //QString m_gridData[ROWS][COLS];
    //QString** data1;
    QStringList* data1;
    int rows;
    int cols;
};

#endif // MYMODEL_H
