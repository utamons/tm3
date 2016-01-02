#pragma once
#include <QString>

enum DayType {
	WORK = 0,	// рабочий день
	OVERTIME,		// переработка
	RESTDAY,		// выходной
	HOLIDAY,		// праздник
	VACATION,		// отпуск
	ILLNESS,		// болезнь
	COMPENSATORY,	// отгул
    ILLNESS_LIST, // закрыто больничным
	NONE			// не определен
};

struct Day {

    Day() {
        type = NONE;
        month = -1;
    }

    bool isEmpty() const { return month == -1; }

    Day(DayType type, int month, int weekday, int mday) :
        type(type), weekday(weekday), mday(mday), month(month)  {
	}

    QString toString() const {return QString::number(mday);}

    DayType type;
    int weekday,mday, month;

};
