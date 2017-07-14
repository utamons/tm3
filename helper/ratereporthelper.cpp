#include "ratereporthelper.h"
#include "tm3util.h"
#include <QtGlobal>

RateReportHelper::RateReportHelper(std::shared_ptr<PeriodHelper> pHelper, QObject *parent) :
	QObject(parent),pHelper(pHelper) {
}

QList<std::pair<QString,QString>> RateReportHelper::getRatesReport() const {
	QList<std::pair<QString,QString>> result;
	QSqlQuery q(getDb());

	q.prepare(
				"select r.id as rid,  a.mins as mins,  r.time as rtime, l.value as val "
				"from actions a, act_rate_link l, rate_rb r "
				"where a.time>=:dt1 and a.time<=:dt2 and a.id=l.act_id and r.id=l.rate_id");

	q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
	q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());

    QMap<Rate,double> report;
	RateListModel rModel;

	execQuery(q, [&rModel,&report,q]() {
        Rate rt = rModel.rateById(getField<int>(q,"rid"));
        double val = report.value(rt,0.0);
       report.insert(rt,val+RateReportHelper::calculateRate(q));
	});

    for (Rate rt : report.keys()) {

        result.append(std::pair<QString,QString>(rt.name.append(" (")
                            .append(rt.unit.name).append(")"),
							QString::number(report.value(rt,0),'f',2)));
	}

	return result;
}

double RateReportHelper::calculateRate(QSqlQuery q) {

	double mins = getField<double>(q,"mins");
	double val = getField<double>(q,"val");
	int rtime =  getField<int>(q,"rtime");

    Q_ASSERT ((rtime > 0 && mins > 0) || rtime == 0);

	double result = 0.0;

	if (rtime > 0 && mins > 0) { // если оценка рассчитывается по времени
		result = val*(mins/rtime);
	} else if (rtime == 0) {
		result = val; // если это оценка не привязанная ко времени
	}

	return result;
}

double RateReportHelper::getRateSum(int rateId) const {
	double result = 0;
	QSqlQuery q(getDb());
	q.prepare(
				"select r.id as rid,  a.mins as mins,  r.time as rtime, l.value as val "
				"from actions a, act_rate_link l, rate_rb r "
				"where a.time>=:dt1 and a.time<=:dt2 and r.id=:rid and a.id=l.act_id and r.id=l.rate_id");

	q.bindValue(":dt1", pHelper->dt1toMinsSinceEpoch());
	q.bindValue(":dt2", pHelper->dt2toMinsSinceEpoch());
	q.bindValue(":rid", rateId);

	execQuery(q, [&result,q]() {
			result += RateReportHelper::calculateRate(q);
	});

	return result;
}
