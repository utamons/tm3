#pragma once

#include <QObject>
#include "periodhelper.h"
#include "dbhelper.h"
#include "dayshelper.h"
#include <memory>

class WorkTimeReportHelper : public QObject
{
    Q_OBJECT
public:
    explicit WorkTimeReportHelper(const std::shared_ptr<PeriodHelper> pHelper, QObject *parent = 0):pHelper(pHelper),QObject(parent){}

    QString workDaysStr() const { return tr("Рабочих дней: ") + QString::number(getWorkDays());}
    QString workTimeStr() const { return tr("Asia-Soft в день: ") + getWorkTime();}
    QString illnessDaysStr() const { return tr("Не закрыто больничным: ") + QString::number(getIllnessDays());}
    QString illnessListDaysStr() const { return tr("Закрыто больничным: ") + QString::number(getIllnessListDays());}
    QString notUsedDaysStr() const { return tr("Неиспользовано отпуска: ") + QString::number(getNotUsedDays());}
    QString overWorkDaysStr() const { return tr("Переработок: ") + QString::number(getOverWorkDays());}
    QString debtDaysStr() const { return tr("Долг Asia-Soft: ") + QString::number(getDebtDays());}

private:
    const std::shared_ptr<PeriodHelper> pHelper;

    int getWorkDays() const; // возвращает рабочее время в днях
    int getIllnessDays() const; // возвращает время болезней в днях
    int getIllnessListDays() const; // дней, закрытых больничным
    int getNotUsedDays() const; // возвращает время неиспользованного отпуска в днях
    int getOverWorkDays() const; // возвращает время переработок, не закрытых отгулами
    int getDebtDays() const; // возвращает количество дней, которые мне должен Asia-Soft, с учётом трёх дней болезней за сезон.

    int getDaysOfType(DayType type) const;

    QString getWorkTime() const; // возвращает среднее время, потраченное на Asia-Soft за день

};
