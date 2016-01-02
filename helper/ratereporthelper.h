#pragma once

#include <QObject>
#include "periodhelper.h"
#include "ratelistmodel.h"
#include "dbhelper.h"
#include <memory>

class RateReportHelper : public QObject
{
	Q_OBJECT
public:
    explicit RateReportHelper(const std::shared_ptr<PeriodHelper> pHelper, QObject *parent = 0);

	QList<std::pair<QString,QString>> getRatesReport() const;
	double getRateSum(int rateId) const;

private:

    const std::shared_ptr<PeriodHelper> pHelper;
	double static calculateRate(QSqlQuery q);
};
