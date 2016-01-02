#include "budjettablemodel.h"
#include "tm3util.h"
#include <QDebug>
#include <QtGlobal>

BudjetTableModel::BudjetTableModel(QObject *parent) :
	QAbstractTableModel(parent) {
	init(BF_ALL);
}

void BudjetTableModel::init(BudjetListFilter filter) {
	bList.clear();
	QString sql = "select id,name,workdays,descendant,illness,correction,min,max,"
				  "dt_beg,dt_end,period,cat_id,rate_id,tag_id from budjet";

	QDate dtCurr = QDate::currentDate();
	QString where = " where dt_end>= :dt_end";

	QSqlQuery q(getDb());
	q.prepare(sql+where+QString(" and period = :period"));

	switch (filter) {
	case BF_ALL:
		q.clear();
		q.prepare(sql+where);
		break;
	case BF_DAY:
		q.bindValue(":period", BF_DAY);
		break;
	case BF_MONTH:
		q.bindValue(":period", BF_MONTH);
		break;
	case BF_QUARTER:
		q.bindValue(":period", BF_QUARTER);
		break;
	case BF_WEEK:
		q.bindValue(":period", BF_WEEK);
		break;
	case BF_YEAR:
		q.bindValue(":period", BF_YEAR);
		break;
	case BF_OLD:
		q.clear();
		where = " where dt_end<";
		where.append(QString::number(toEpochMins(dtCurr)));
		q.prepare(sql.append(where));
		break;
	}

	q.bindValue(":dt_end", toEpochMins(dtCurr));

	execQuery(q, [q,this]() {
		Budjet b (
					getField<int>(q,"id"),
					getField<QString>(q,"name"),
					getField<int>(q,"workdays")==1,
					getField<int>(q,"descendant")==1,
					getField<int>(q,"illness")==1,
					getField<int>(q,"correction")==1,
					getField<int>(q,"min"),
					getField<int>(q,"max"),
					toDate(getField<long>(q,"dt_beg")),
					toDate(getField<long>(q,"dt_end")),
					periodFromInt(getField<int>(q,"period")),
					getField<int>(q,"cat_id"),
					getField<int>(q,"rate_id"),
					getField<int>(q,"tag_id"));
		bList.append(b);
	});
}

void BudjetTableModel::addRow(Budjet b) {
	QSqlQuery q(getDb());
	q.prepare("insert into budjet (name,workdays,descendant,illness,correction,"
			  "min,max,dt_beg,dt_end,period,cat_id,rate_id,tag_id) values ("
			  ":name,:workdays,:descendant,:illness,:correction,:min,:max,"
			  ":dt_beg,:dt_end,:period,:cat_id,:rate_id,:tag_id)");
	q.bindValue(":name", b.name);
	q.bindValue(":workdays", b.workdays ? 1 : 0);
	q.bindValue(":descendant", b.descendant ? 1 : 0);
	q.bindValue(":illness", b.illness ? 1 : 0);
	q.bindValue(":correction", b.correction ? 1 : 0);
	q.bindValue(":min", b.min);
	q.bindValue(":max", b.max);
	q.bindValue(":dt_beg", toEpochMins(b.dtBegin));
	q.bindValue(":dt_end", toEpochMins(b.dtEnd));
	q.bindValue(":period", b.period);

	if ( b.catId > 0 )
		q.bindValue(":cat_id", b.catId);
	if (b.rateId > 0 )
		q.bindValue(":rate_id",  b.rateId);
	if (b.tagId > 0)
		q.bindValue(":tag_id",  b.tagId );

	execQuery(q);

	b.id = q.lastInsertId().toInt();

	beginInsertRows(QModelIndex(), bList.length(), bList.length());
	bList.append(b);
	endInsertRows();
}

void BudjetTableModel::updateRow(Budjet b, const QModelIndex& row) {
	if (row.isValid()) {
		QSqlQuery q(getDb());

		q.prepare("update budjet set "
				  "name=:name,"
				  "workdays=:workdays,"
				  "descendant=:descendant,"
				  "illness=:illness,"
				  "correction=:correction,"
				  "min=:min,"
				  "max=:max,"
				  "dt_beg=:dt_beg,"
				  "dt_end=:dt_end,"
				  "period=:period,"
				  "cat_id=:cat_id,"
				  "rate_id=:rate_id,"
				  "tag_id=:tag_id  where id=:id");

		q.bindValue(":name", b.name);
		q.bindValue(":workdays", b.workdays ? 1 : 0);
		q.bindValue(":descendant", b.descendant ? 1 : 0);
		q.bindValue(":illness", b.illness ? 1 : 0);
		q.bindValue(":correction", b.correction ? 1 : 0);
		q.bindValue(":min", b.min);
		q.bindValue(":max", b.max);
		q.bindValue(":dt_beg", toEpochMins(b.dtBegin));
		q.bindValue(":dt_end", toEpochMins(b.dtEnd));
		q.bindValue(":period", b.period);
		if ( b.catId > 0 )
			q.bindValue(":cat_id", b.catId);
		if (b.rateId > 0 )
			q.bindValue(":rate_id",  b.rateId);
		if (b.tagId > 0)
			q.bindValue(":tag_id",  b.tagId);
		q.bindValue(":id",  b.id);

		execQuery(q);

		bList.replace(row.row(),b);
	}
}

SqlQueryStatus BudjetTableModel::deleteRow(const QModelIndex& row) {
	SqlQueryStatus result = SQL_OK;
	if (row.isValid()) {
		QSqlQuery q(getDb());
		int id = bList[row.row()].id;
		q.prepare("delete from budjet where id=:id");
		q.bindValue(":id", id);
		result = execQuery(q,true);
		if (result == SQL_OK) {
			beginRemoveRows(QModelIndex(), row.row(), row.row());
			bList.removeAt(row.row());
			endRemoveRows();
		}
	}
	return result;
}

int BudjetTableModel::rowCount(const QModelIndex&) const {
	return bList.size();
}

int BudjetTableModel::columnCount(const QModelIndex&) const {
	return 4;
}

QVariant BudjetTableModel::data(const QModelIndex& index, int role) const {
	QVariant result;
	if (index.isValid() && role == Qt::DisplayRole) {
		Q_ASSERT(index.column() < 4);
		Budjet b = bList[index.row()];
		switch (index.column()) {
		case 0:
			result = QVariant(b.name);
			break;
		case 1:
			result = QVariant(period2QString(b.period));
			break;
		case 2:
			result = QVariant(b.dtEnd);
			break;
		case 3:
			if (b.catId > 0)
				result = QVariant(tr("категория"));
			else if (b.rateId > 0)
				result = QVariant(tr("оценка"));
			else if (b.tagId > 0)
				result = QVariant(tr("метка"));
			else
				result = QVariant(tr("неопределено"));
		}
	}
	return result;
}

QVariant BudjetTableModel::headerData(int section, Qt::Orientation, int role) const {
	QVariant result;
	if (role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			result = QVariant(tr("Название"));
			break;
		case 1:
			result = QVariant(tr("Период"));
			break;
		case 2:
			result = QVariant(tr("Дата"));
			break;
		case 3:
			result = QVariant(tr("Тип"));
			break;
		}
	}
	return result;
}

QModelIndex BudjetTableModel::index(int row, int column, const QModelIndex& parent) const {
    return !parent.isValid() && bList.length() > 0 && row < bList.length() ?
                createIndex(row, column, const_cast<Budjet*>( &(bList.at(row)) )) : QModelIndex();
}

Budjet BudjetTableModel::budjetFromIndex(const QModelIndex& index) const {
    if (index.isValid())
        return bList[index.row()];
    else
        return Budjet();
}

QList<Budjet> BudjetTableModel::getCatBudjetList() const {
    QList<Budjet> result;

    for(auto b : bList) {
        if (b.catId > 0) {
            result.append(b);
        }
    }

    return result;
}

QList<Budjet> BudjetTableModel::getTagBudjetList() const {
    QList<Budjet> result;

    for(auto b : bList) {
        if (b.tagId > 0) {
            result.append(b);
        }
    }

    return result;
}

QList<Budjet> BudjetTableModel::getRateBudjetList() const {
    QList<Budjet> result;

    for(auto b : bList) {
        if (b.rateId > 0) {
            result.append(b);
        }
    }

    return result;
}

bool BudjetTableModel::hasName(QString name, int selfId) {
	QSqlQuery q(getDb());
	q.prepare("select count(id) as cnt from budjet where name=:name and id<>:id");
	q.bindValue(":name",name);
	q.bindValue(":id",selfId);
	bool result = false;

	execQuery(q,[q,&result] {
		result = getField<int>(q,"cnt") > 0;
	});
	return result;
}

BudjetPeriod BudjetTableModel::periodFromInt(int p) {
    BudjetPeriod result;
    Q_ASSERT_X(p > -1 && p < 6,"BudjetTableModel::periodFromInt", "Invalid period code!");
    switch (p) {
    case 0:
        result = DAY;
        break;
    case 1:
        result = WEEK;
        break;
    case 2:
        result = MONTH;
        break;
    case 3:
        result = QUARTER;
        break;
    case 4:
        result = YEAR;
        break;
    case 5:
        result = WORKDAY;
        break;
    }
    return result;
}

QString BudjetTableModel::period2QString(const BudjetPeriod bp) {
	QString result;
	switch (bp) {
	case DAY:
		result = tr("день");
		break;
	case MONTH:
		result = tr("месяц");
		break;
	case QUARTER:
		result = tr("квартал");
		break;
	case WEEK:
		result = tr("неделя");
		break;
	case YEAR:
		result = tr("год");
		break;
	case WORKDAY:
		result = tr("раб.день");
		break;
	}
	return result;
}
