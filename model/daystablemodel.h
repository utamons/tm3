#pragma once

#include <QAbstractTableModel>
#include "day.h"

class DaysTableModel: public QAbstractTableModel {
Q_OBJECT
public:
	explicit DaysTableModel(QObject *parent = 0);
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
    Day dayFromIndex(const QModelIndex &idx);
	void init(int month);

private:
    Day days[6][7];
};
