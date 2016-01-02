#pragma once

#include <QDate>
#include "day.h"

class DaysHelper
{
public:
    explicit DaysHelper(int year = QDate::currentDate().year());

    DayType getType(QDate day) const;
    void setType(Day date, DayType type) const;

private:
    QList<QDate> holydays;
    int year;
};
