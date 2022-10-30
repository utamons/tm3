#include "tagreporthelper.h"
#include "dbhelper.h"
#include "tm3util.h"

TagReportHelper::TagReportHelper(const std::shared_ptr<PeriodHelper> pHelper, QObject *parent) :
	QObject(parent), pHelper(pHelper)  {
}

QList<std::pair<QString,QString>> TagReportHelper::getTagsReport() const {
	QList<std::pair<QString,QString>> result;
	QString sql =
			"select t.name as nm, sum(mins) as sm from actions a, tag_rb t,"
			"act_tag_link l where a.id=l.act_id and t.id=l.tag_id  "
			"and a.time>=:dt1 and a.time<=:dt2 group by t.id order by sm desc";

	QSqlQuery q(getDb());
	q.prepare(sql);
	q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
	q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());

    execQuery(q, [&q,&result]() {
		auto nm = field<QString>(q,"nm");
		auto sm = timeFromMins(field<long>(q,"sm"));
		result.append(std::pair<QString,QString>(nm,sm));
	});

	return result;
}

long TagReportHelper::getTagTime(int tagId) const {
	long result = 0;
	QString sql =
			"select sum(a.mins) as sm from actions a, tag_rb t,"
			"act_tag_link l where a.id=l.act_id and t.id=l.tag_id and t.id=:id and a.time>=:dt1 and a.time<=:dt2";
	QSqlQuery q(getDb());
	q.prepare(sql);
	q.bindValue(":id",tagId);
	q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
	q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());

    execQuery(q, [&q,&result]() {
		result = field<long>(q,"sm");
	});

	return result;
}
