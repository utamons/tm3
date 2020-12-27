#include "periodhelper.h"
#include <QMessageBox>
#include <QApplication>

PeriodHelper::PeriodHelper(QDateEdit *dtFirst, QDateEdit *dtSecond) :
        MONTH_PERIOD(-1), YEAR_PERIOD(-2) {
	this->dtFirst = dtFirst;
	this->dtSecond = dtSecond;
    dtChangeProcess = false;
    period = 1;
    valid = true;
}

void PeriodHelper::dt1Change() {
	if (!dtChangeProcess) {
		if (dtFirst->date() >= dtSecond->date()) {
			dtFirst->setDate(dtSecond->date());
			period = 1;
		} else {
            period = static_cast<int>(dtFirst->date().daysTo(dtSecond->date())) + 1;
		}
		setNameByDates();
		emit makeReport();
	}
}

void PeriodHelper::dt2Change() {
	if (!dtChangeProcess) {
		if (dtFirst->date() >= dtSecond->date()) {
			dtFirst->setDate(dtSecond->date());
			period = 1;
		} else {
            period = static_cast<int>(dtFirst->date().daysTo(dtSecond->date())) + 1;
		}
		setNameByDates();
		emit makeReport();
	}
}

void PeriodHelper::setNameByDates() {
	name.clear();
    int period = static_cast<int>(dtFirst->date().daysTo(dtSecond->date()));
	if (period == 6)
		name = tr("week ");
	name.append(dtFirst->date().toString("dd.MM.yyyy"));
	if (period > 0)
		name.append(" - ").append(dtSecond->date().toString("dd.MM.yyyy"));
}

void PeriodHelper::fwd() {
	dtChangeProcess = true;

	if (period == MONTH_PERIOD) {
		QDate dtNext = dtSecond->date().addDays(1);
		setPeriod(dtNext, dtNext.day(), dtNext.daysInMonth(), MONTH_PERIOD);
		name = getMonthName(dtNext).append(dtNext.toString(" yyyy"));
	} else if (period == YEAR_PERIOD) {
		QDate dtNext = dtSecond->date().addDays(1);
		setPeriod(dtNext, dtNext.dayOfYear(), dtNext.daysInYear(), YEAR_PERIOD);
		name = dtNext.toString("yyyy").append(tr(" year"));
	} else {
		dtFirst->setDate(dtFirst->date().addDays(period));
		dtSecond->setDate(dtSecond->date().addDays(period));
		setNameByDates();
	}
	emit makeReport();
	dtChangeProcess = false;
}

void PeriodHelper::back() {
	dtChangeProcess = true;

	if (period == MONTH_PERIOD) {
		QDate dtPrev = dtFirst->date().addDays(-1);
		setPeriod(dtPrev, dtPrev.day(), dtPrev.daysInMonth(), MONTH_PERIOD);
		name = getMonthName(dtPrev).append(dtPrev.toString(" yyyy"));
	} else if (period == YEAR_PERIOD) {
		QDate dtPrev = dtFirst->date().addDays(-1);
		setPeriod(dtPrev, dtPrev.dayOfYear(), dtPrev.daysInYear(), YEAR_PERIOD);
		name = dtPrev.toString("yyyy").append(tr(" year"));
	} else {
		dtFirst->setDate(dtFirst->date().addDays(-period));
		dtSecond->setDate(dtSecond->date().addDays(-period));
		setNameByDates();
	}
	emit makeReport();
	dtChangeProcess = false;
}

void PeriodHelper::day() {
	dtChangeProcess = true;
	setPeriod(QDate::currentDate(), 1, 1, 1);
	setNameByDates();
	emit makeReport();
	dtChangeProcess = false;
}

void PeriodHelper::week() {
	dtChangeProcess = true;
	setPeriod(QDate::currentDate(), QDate::currentDate().dayOfWeek(), 7, 7);
	setNameByDates();
	emit makeReport();
	dtChangeProcess = false;
}

void PeriodHelper::setPeriod(QDate baseDate, int day, int days, int period) {
	dtFirst->setDate(baseDate.addDays(-day + 1));
	dtSecond->setDate(baseDate.addDays(days - day));
	this->period = period;
}

void PeriodHelper::month() {
	dtChangeProcess = true;
	setPeriod(QDate::currentDate(), QDate::currentDate().day(),
			QDate::currentDate().daysInMonth(), MONTH_PERIOD);
	name = getMonthName(QDate::currentDate()).append(QDate::currentDate().toString(" yyyy"));
	emit makeReport();
	dtChangeProcess = false;
}

void PeriodHelper::year() {
	dtChangeProcess = true;
	setPeriod(QDate::currentDate(), QDate::currentDate().dayOfYear(),
			QDate::currentDate().daysInYear(), YEAR_PERIOD);
	name = QDate::currentDate().toString("yyyy").append(tr(" year"));
	emit makeReport();
	dtChangeProcess = false;
}

void PeriodHelper::checkValid() const {
	if (!valid) {
		QMessageBox::critical(nullptr, tr("Error"), QObject::tr("Trying to call invalid PeriodHelper!"));
		QApplication::exit(1);
	}
}

int PeriodHelper::periodMins() const {
	checkValid();
	const int MINS_IN_DAY = 1440;
    return static_cast<int>(dtFirst->date().daysTo(dtSecond->date()) + 1) * MINS_IN_DAY;
}

QString PeriodHelper::getMonthName(QDate dt) {
	QString result;
	switch (dt.month()) {
	case 1:
		result = tr("January");
		break;
	case 2:
		result = tr("February");
		break;
	case 3:
		result = tr("March");
		break;
	case 4:
		result = tr("April");
		break;
	case 5:
		result = tr("May");
		break;
	case 6:
		result = tr("June");
		break;
	case 7:
		result = tr("July");
		break;
	case 8:
		result = tr("August");
		break;
	case 9:
		result = tr("September");
		break;
	case 10:
		result = tr("October");
		break;
	case 11:
		result = tr("November");
		break;
	case 12:
		result = tr("December");
		break;
	}
	return result;
}

int PeriodHelper::dt1toMinsSinceEpoch() const {
	checkValid();
    return static_cast<int>(dtFirst->date().startOfDay().toMSecsSinceEpoch())/60000;
}

int PeriodHelper::dt2toMinsSinceEpoch() const {
	checkValid();
    return static_cast<int>(dtSecond->date().startOfDay().addDays(1).toMSecsSinceEpoch()-1)/60000;
}

void PeriodHelper::parentDead() {
	valid = false;
}


