#include "mymodel.h"

MyModel::MyModel(QObject *parent) :
  QAbstractTableModel(parent)
{   
}

MyModel::MyModel(QObject *parent, QStringList* data, int rowCount, int colCount) :
  QAbstractTableModel(parent),
  tableData(data),
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

QVariant MyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            return tableTitle->at(section); 
        }
    }
    return QVariant();
}

bool MyModel::setData(QStringList* data, int rowCount, int colCount)
{
    beginResetModel();
    delete tableData;
    tableData = data;
    rows = rowCount;
    cols = colCount;
    endResetModel();
}

void MyModel::setTitle(QStringList* title)
{
  delete tableTitle;
  tableTitle = title;
}

QVariant MyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
      return tableData->at(index.row() * cols + index.column());
    }
    return QVariant();
}




