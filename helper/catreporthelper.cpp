#include "catreporthelper.h"
#include "dbhelper.h"

CatReportHelper::CatReportHelper(const std::shared_ptr<PeriodHelper> pHelper) :
    pHelper(pHelper) {
}

QList<std::pair<QString,QString>> CatReportHelper::getRootCatReport(QString rootName) const {
    QList<std::pair<QString,QString>> result;
    qDebug()<<"Category "<<rootName;

    QList<int> ql;
    ql = childLeafIds(rootId(rootName), ql);
    qDebug()<<"Leaves found "<<ql.size();

    if (ql.size() > 0) {
        QString sql =
                "select c.name as nm, sum(mins) as sm from actions a, cat_rb c "
                "where a.cat_id=c.id and a.cat_id in (";

        foreach (int id, ql){
            sql.append(QString::number(id)).append(",");
        }

		sql.chop(1);

        sql.append(
                    ") and a.time>=:dt1 and a.time<=:dt2 group by a.cat_id order by sm desc");

        QSqlQuery q(getDb());
        q.prepare(sql);
        q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
        q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());

        execQuery(q, [&q,&result]() {
            auto nm = field<QString>(q,"nm");
            auto sm = timeFromMins(field<int>(q,"sm"));
            result.append(std::pair<QString,QString>(nm,sm));
        });

    }

	return result;
}

QList<QString> CatReportHelper::getRootCatList() const {
	QSqlQuery q(getDb());
    QList<QString> result;
    q.prepare("select name from cat_rb where parent_id=0 and name <> 'RECYCLE'");
    execQuery(q,[&q,&result]() {
			auto name = field<QString>(q,"name");
            result.append(name);
		});
    return result;
}

QString CatReportHelper::getTotalTime() const {
    QSqlQuery q(getDb());
    double total;

    q.prepare(
                "select sum(mins) as total from actions where time>=:dt1 and time<=:dt2");
    q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
    q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());

    execQuery(q, [&q,&total]() {
        total = field<double>(q,"total");
    });

    auto pc = total / pHelper->periodMins() * 100.0;

    return timeFromMins(total) + " ("  + QString::number(pc) +  "%)";
}

long CatReportHelper::getTotalTime(QString catName) const {
    int rId = rootId(catName);

    QList<int> ql;
    ql = childLeafIds(rId, ql);
    ql.append(rId);

    QString sql =
            "select sum(mins) as sm from actions a "
            "where a.cat_id in (";

    for (int id : ql){
        sql = sql + QString::number(id) + ",";
    }

    sql.chop(1); // Отстригли последнюю запятую.

    sql.append(
                ") and a.time>=:dt1 and a.time<=:dt2");

    QSqlQuery q(getDb());
    q.prepare(sql);
    q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
    q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());

    long result;
    execQuery(q, [&q,&result]() {
        result = field<long>(q,"sm");
    });

    return result;
}

long CatReportHelper::getCatTime(int catId) const {
    QSqlQuery q(getDb());
    auto total = 0L;

    q.prepare(
                "select sum(mins) as total from actions where time>=:dt1 and time<=:dt2 and catId=:id");
    q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
    q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());
    q.bindValue(":id",catId);

    execQuery(q, [&q,&total]() {
        total = field<long>(q,"total");
    });

    return total;
}

QList<int> CatReportHelper::childLeafIds(int parentId, QList<int> &ql) const {
    QSqlQuery q(getDb());
    q.prepare("select id,name from cat_rb where parent_id=:parent_id");
    q.bindValue(":parent_id", parentId);

    execQuery(q, [&q,&ql,this]() {
        childLeafIds(field<int>(q,"id"),ql);
    });

    ql.append(parentId);

    return ql;
}

int CatReportHelper::rootId(QString rootName) const {
    QSqlQuery q(getDb());
    q.prepare("select id from cat_rb where name=:name");
    q.bindValue(":name", rootName);

    int rootId;
    execQuery(q, [&q,&rootId]() {
        rootId = field<int>(q,"id");
    });

    return rootId;
}
