#include "mymodel.h"

MyModel::MyModel(QObject *parent) :
  QAbstractTableModel(parent)
{
}

MyModel::MyModel(QObject *parent, QStringList* data, int rowCount, int colCount) :
  QAbstractTableModel(parent),
  data1(data),
  rows(rowCount),
  cols(colCount)
{
}

int MyModel::rowCount(const QModelIndex &/*parent*/) const
{
    return rows;
}

int MyModel::columnCount(const QModelIndex &/*parent*/) const
{
  return cols;
}

bool MyModel::setData(QStringList* data, int rowCount, int colCount)
{
    beginResetModel();
    delete data1;
    data1 = data;
    rows = rowCount;
    cols = colCount;
    endResetModel();
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
      return data1->at(index.row() * cols + index.column());
    }
    return QVariant();
}




