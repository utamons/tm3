#pragma once
#include <QAbstractListModel>
#include <QList>
#include <QDebug>
#include "dbhelper.h"
#include "insertfunctor.h"
#include "updatefunctor.h"
#include "deletefunctor.h"

template<typename T>

class BaseListModel : public QAbstractListModel {

public:
    explicit BaseListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return parent.isValid() ? 0 : rowList.length();
    }

    QVariant data(const QModelIndex &index, int role) const override  {
        QVariant result = QVariant();
        if (index.isValid()) {
            T obj = rowList[index.row()];

            if ((role == Qt::DisplayRole || role == Qt::EditRole) && index.column() == 0 && !obj.isEmpty()) {
                result = QVariant(obj.toString());
            }
        }
        return result;
    }

    T entityFromIndex(const QModelIndex& index) const {
        if (index.isValid())
            return  rowList[index.row()];
        else
            return T();
    }

    QModelIndex index(int row, int column,
                      const QModelIndex &parent=QModelIndex()) const override {
        return !parent.isValid() && rowList.length() > 0
                && row < rowList.length() && row > -1 ?
                    createIndex( row, column, const_cast<T*>( &(rowList.at(row)) )) : QModelIndex();
    }

    void insert(T entity) {
        insertFunc(entity);
        beginInsertRows(QModelIndex(), rowList.length(), rowList.length());
        rowList.append(entity);
        endInsertRows();
    }

    void update(T enity, const QModelIndex &row) {
        if (row.isValid()) {
            updateFunc(enity);
            rowList.replace(row.row(),enity);
        }
    }

    SqlQueryStatus remove(const QModelIndex &idx) {
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

protected:

    InsertFunctor<T> insertFunc;
    UpdateFunctor<T> updateFunc;
    DeleteFunctor<T> deleteFunc;
    QList<T> rowList;
};

