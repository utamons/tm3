#pragma once

#include <QAbstractTableModel>
#include "budjet.h"
#include "dbhelper.h"

enum BudjetListFilter {
  BF_DAY=0,
  BF_WEEK,
  BF_MONTH,
  BF_QUARTER,
  BF_YEAR,
  BF_ALL,
  BF_OLD
};

class BudjetTableModel: public QAbstractTableModel {
Q_OBJECT
public:
	explicit BudjetTableModel(QObject *parent = 0);

	void init(BudjetListFilter filter=BF_ALL);
    void addRow(Budjet b);
    void updateRow(Budjet b,const QModelIndex &row);
    SqlQueryStatus deleteRow(const QModelIndex &row);
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex&) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation, int role) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    Budjet budjetFromIndex(const QModelIndex &index) const;
    QList<Budjet> getCatBudjetList() const;
    QList<Budjet> getTagBudjetList() const;
    QList<Budjet> getRateBudjetList() const;

	static bool hasName(QString name, int selfId);
	static QString period2QString(const BudjetPeriod bp);
	static BudjetPeriod periodFromInt(int p);

private:
    QList<Budjet> bList;
};

