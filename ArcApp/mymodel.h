#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <Vector>
#include <qDebug>

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent);
    MyModel(QObject *parent, QStringList* data, int rowCount, int colCount);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool setData(QStringList* data, int rowCount, int colCount);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
    QStringList* data1 = NULL;
    int rows = 0;
    int cols = 0;
signals:
    void editCompleted();
};

#endif // MYMODEL_H
