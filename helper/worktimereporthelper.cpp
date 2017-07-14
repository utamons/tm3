#include "worktimereporthelper.h"
#include "catreporthelper.h"
#include "QDebug"
#include "tm3util.h"

int WorkTimeReportHelper::getWorkDays() const {
   return getDaysOfType(WORK);
}

int WorkTimeReportHelper::getIllnessDays() const {
    return getDaysOfType(ILLNESS);
}

int WorkTimeReportHelper::getIllnessListDays() const {
    return getDaysOfType(ILLNESS_LIST);
}

int WorkTimeReportHelper::getNotUsedDays() const {
    // льготные дни отпуска в АС:
    const int VAC_DAYS = QDate::currentDate().year() > 2016 ? 31+3 : 31;
    int vac_days = (pHelper->lastDate().year()-pHelper->firstDate().year()+1)*VAC_DAYS;
    int real_days = getDaysOfType(VACATION);
    return vac_days - real_days;
}

int WorkTimeReportHelper::getOverWorkDays() const {
    int over = getDaysOfType(OVERTIME);
    int comp = getDaysOfType(COMPENSATORY);
    return over-comp;
}

int WorkTimeReportHelper::getDebtDays() const {
    int ill =  getIllnessDays();
    int overIllness = ill <= 12 ? 0 : ill - 12;
    return getOverWorkDays()+getNotUsedDays()-overIllness;
}

int WorkTimeReportHelper::getDaysOfType(DayType type) const {
    QDate firstDate = pHelper->firstDate();
    QDate lastDate = pHelper->lastDate();
    int days = 0;

    for (QDate day = firstDate; day <= lastDate && day <= QDate::currentDate(); day = day.addDays(1)) {
        const DaysHelper helper(day.year());
        if (helper.getType(day) == type)
            days++;
    }

    return days;
}

QString WorkTimeReportHelper::getWorkTime() const {
    const CatReportHelper crHelper(pHelper);
    int wd = getWorkDays();
    return wd == 0 ? timeFromMins(0) :  timeFromMins(crHelper.getTotalTime("Asia-Soft")/wd);
}


