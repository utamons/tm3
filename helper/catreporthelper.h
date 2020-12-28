#pragma once

#include "tm3util.h"
#include "periodhelper.h"
#include <memory>

class CatReportHelper
{
public:
    explicit CatReportHelper(const std::shared_ptr<PeriodHelper> pHelper);

	QList<std::pair<QString,QString>> getRootCatReport(QString rootName) const;
    QList<QString> getRootCatList() const;
	QString getTotalTime() const;
    long getTotalTime(QString catName) const;
	long getCatTime(int catId) const;

private:
    const std::shared_ptr<PeriodHelper> pHelper;

	QList<int> childLeafIds(int parentId, QList<int> &ql) const;
    int rootId(QString rootName) const;
};
