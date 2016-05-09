#include "mymodel.h"

MyModel::MyModel(QObject *parent, QStringList* data, int rowCount, int colCount) :
  QAbstractTableModel(parent),
  data1(data),
  rows(rowCount),
  cols(colCount)
{
    // data1 = data;
    // rows = rowCount;
    // cols = colCount;
}

int MyModel::rowCount(const QModelIndex &/*parent*/) const
{
    return rows;
}

int MyModel::columnCount(const QModelIndex &/*parent*/) const
{
  return cols;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
      return data1->at(index.row() * cols + index.column());
      // if (index.row() == 0 && index.column() == 0)
      // {
      //   return data1->at(0);
      // }
      // if (index.row() == 0 && index.column() == 1)
      // {
      //   return data1->at(1);
      // }
      // if (index.row() == 1 && index.column() == 0)
      // {
      //   return data1->at(2);
      // }
      // if (index.row() == 1 && index.column() == 1)
      // {
      //   return data1->at(3);
      // }
      //return (data1.at(index.row())).at(index.column());//data1[index.row()][index.column()];
    }
    return QVariant();
}




