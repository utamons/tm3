#include "dayshelper.h"
#include "dbhelper.h"
#include <QtGlobal>
#include <QDateTime>

DaysHelper::DaysHelper(int year) {
	holydays.append(QDate(year, 1, 1)); // Новый год
	holydays.append(QDate(year, 1, 2));
	holydays.append(QDate(year, 1, 7)); // Рождество
	holydays.append(QDate(year, 3, 8)); // 8 марта
	holydays.append(QDate(year, 3, 21)); // Наурыз
	holydays.append(QDate(year, 3, 22));
	holydays.append(QDate(year, 3, 23));
	holydays.append(QDate(year, 5, 1)); // День единства народов Казахстана
	holydays.append(QDate(year, 5, 7)); // День защитника отечества
	holydays.append(QDate(year, 5, 9)); // День победы
	holydays.append(QDate(year, 7, 6)); // День столицы
	holydays.append(QDate(year, 8, 30)); // День конституции
	holydays.append(QDate(year, 12, 1)); // День Папы
	holydays.append(QDate(year, 12, 16)); // День независимости
	holydays.append(QDate(year, 12, 17)); // День независимости
	this->year = year;
}

DayType DaysHelper::getType(QDate day) const {
	Q_ASSERT(day.year() == year);
	QDateTime dtm(day);
	DayType result = NONE;

	// Ищем в базе - не отмечен ли день особо
	QSqlQuery q(getDb());
	q.prepare("select type from days where dt = :dt");
	q.bindValue(":dt", dtm.toMSecsSinceEpoch() / 60000L);
	execQuery(q, [q,&result] {
		switch (getField<int>(q,"type")) {
			case 0:
			result = WORK;
			break;
			case 1:
			result = OVERTIME;
			break;
			case 2:
			result = RESTDAY;
			break;
			case 3:
			result = HOLIDAY;
			break;
			case 4:
			result = VACATION;
			break;
			case 5:
			result = ILLNESS;
			break;
			case 6:
			result = COMPENSATORY;
            break;
            case 7:
            result = ILLNESS_LIST;
			break;
		}
	});

	// Проверяем по списку праздников
	if (result == NONE) {
		foreach (QDate dt, holydays){
		if (dt == day) {
			result = HOLIDAY;
			break;
		}
	}
}

// Проверяем - не является ли день выходным
	if (result == NONE && day.dayOfWeek() > 5) {
		result = RESTDAY;
	}

	return result == NONE ? WORK : result;
}

void DaysHelper::setType(Day day, DayType type) const {
	QSqlQuery q(getDb());
    QDate date(year,day.month,day.mday);
	QDateTime dtm(date);
	int dtMins = dtm.toMSecsSinceEpoch() / 60000L;
	bool found = false;

	q.prepare("select count(type) as cnt from days where dt = :dt");
	q.bindValue(":dt", dtMins);
	execQuery(q, [q,&found] {
		found = (getField<int>(q,"cnt") > 0);
	});

	if (found) {
		q.prepare("update days set type = :type where dt = :dt");
	} else {
		q.prepare("insert into days (dt,type) values (:dt,:type)");
	}
	q.bindValue(":type", type);
	q.bindValue(":dt", dtMins);
	execQuery(q);
}
