#pragma once

#include <QObject>
#include "periodhelper.h"
#include <memory>



class BudjetReportHelper : public QObject {
	Q_OBJECT
public:
    explicit BudjetReportHelper(const std::shared_ptr<PeriodHelper> pHelper, QObject *parent = 0);

    QList<std::pair<QString,QString>> getCatBudjetReport() const;
    QList<std::pair<QString,QString>> getTagBudjetReport() const;
    QList<std::pair<QString,QString>> getRateBudjetReport() const;

private:
    const std::shared_ptr<PeriodHelper> pHelper;
};
