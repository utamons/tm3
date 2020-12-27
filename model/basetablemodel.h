#pragma once
#include <QAbstractTableModel>
#include <QtGlobal>
#include "dbhelper.h"
#include "insertfunctor.h"
#include "updatefunctor.h"
#include "deletefunctor.h"

template <typename T>

class BaseTableModel : public QAbstractTableModel {

public:

    BaseTableModel(QObject *parent) :  QAbstractTableModel(parent) {
        colCount = 0;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return parent.isValid() ? 0 : rowList.length();
    }

    int columnCount(const QModelIndex &parent) const override {
        return parent.isValid() ? 0 : colCount;
    }

    virtual QVariant data(const QModelIndex &index, int role) const override {
        QVariant result;
        if (index.isValid() && role == Qt::DisplayRole) {
            Q_ASSERT(index.column() < colCount);
            result = rowList[index.row()].toTableRow()[index.column()];
        }
        return result;
    }

	virtual QVariant headerData(int section, Qt::Orientation orientation,  int role) const override = 0;

    QModelIndex index(int row, int column,
                                         const QModelIndex &parent=QModelIndex()) const override {
        return !parent.isValid() && rowList.length() > 0
                && row < rowList.length() ?
                    createIndex(row, column, const_cast<T*>( &(rowList.at(row)) )) : QModelIndex();
    }

    T entityFromIndex(const QModelIndex &index) const {
        if (index.isValid())
            return rowList[index.row()];
        else
            return T();
    }

    virtual void insert(T entity) {
        insertFunc(entity);
        beginInsertRows(QModelIndex(), rowList.length(), rowList.length());
        rowList.append(entity);
        endInsertRows();
    }

    virtual void update(T enity, const QModelIndex &row) {
        if (row.isValid()) {
            updateFunc(enity);
            rowList.replace(row.row(),enity);
        }
    }

    virtual SqlQueryStatus remove(const QModelIndex &idx) {
        SqlQueryStatus result = SQL_OK;
        if (idx.isValid()) {
            result = deleteFunc(idx.row());

            if (result == SQL_OK) {
                beginRemoveRows(QModelIndex(),idx.row(),idx.row());
                rowList.removeAt(idx.row());
                endRemoveRows();
            }
        }
        return result;
    }

    QModelIndex lastIndex(int column) const {
        int maxRow = rowList.length() - 1;
        return maxRow < 0 ?
                    QModelIndex() : createIndex(maxRow, column,  const_cast <T*>(&rowList.last()) );
    }

protected:

    InsertFunctor<T> insertFunc;
    UpdateFunctor<T> updateFunc;
    DeleteFunctor<T> deleteFunc;
    QList<T> rowList;
    int colCount;

};
