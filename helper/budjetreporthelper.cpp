#include "budjetreporthelper.h"

BudjetReportHelper::BudjetReportHelper(const std::shared_ptr<PeriodHelper>, QObject *parent) :
	QObject(parent), pHelper(pHelper) {
}

QList<std::pair<QString,QString>> BudjetReportHelper::getCatBudjetReport() const {
	QList<std::pair<QString,QString>> result;
	result.append(std::pair<QString,QString>("тест","99.99"));
	return result;
}

QList<std::pair<QString,QString>> BudjetReportHelper::getTagBudjetReport() const {
	QList<std::pair<QString,QString>> result;
	result.append(std::pair<QString,QString>("тест","99.99"));
	return result;
}

QList<std::pair<QString,QString>> BudjetReportHelper::getRateBudjetReport() const {
	QList<std::pair<QString,QString>> result;
	result.append(std::pair<QString,QString>("тест","99.99"));
	return result;
}
