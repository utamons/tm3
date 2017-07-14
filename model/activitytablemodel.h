#pragma once

#include <basetablemodel.h>
#include "activity.h"
#include "dbhelper.h"

class ActivityTableModel: public BaseTableModel<Activity> {
Q_OBJECT
public:
	explicit ActivityTableModel(QObject *parent = 0) :
			BaseTableModel<Activity>(parent) {
		colCount = 4;
	}

	QVariant headerData(int section, Qt::Orientation, int role) const override;

	void init(QDateTime begin, QDateTime end);

    void insert(Activity act) override;
    void update(Activity act, const QModelIndex &row) override;
    SqlQueryStatus remove (const QModelIndex &row) override;

    static bool isCrossTime(QDateTime beginTime, int mins, int selfId = 0);
    static QDateTime getLastTime();
    void fillData(Activity &act);

private:
    QDate currentDate;

};
