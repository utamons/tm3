#pragma once

#include <QObject>
#include "periodhelper.h"
#include <memory>

class TagReportHelper : public QObject
{
	Q_OBJECT
public:
    explicit TagReportHelper(const std::shared_ptr<PeriodHelper> pHelper, QObject *parent = nullptr);

	QList<std::pair<QString,QString>> getTagsReport() const;
	long getTagTime(int tagId) const;

private:
    const std::shared_ptr<PeriodHelper> pHelper;
};
