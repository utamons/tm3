#pragma once

#include <QDate>
#include "rate.h"
#include "category.h"
#include "unit.h"

enum BudjetPeriod {
	DAY=0,WEEK,MONTH,QUARTER,YEAR,WORKDAY
};

struct Budjet  {

     Budjet() {
        catId = -1;
        rateId = -1;
        tagId = -1;
        period=DAY;
        workdays = false;
        descendant = false;
        illness = false;
        correction = false;
	}

	Budjet(int id,QString name, bool workdays, bool descendant, bool illness, bool correction,
			int min, int max, QDate dtBegin, QDate dtEnd, BudjetPeriod period ,
            int catId = 0, int rateId = 0, int tagId = 0) {
		this->id = id;
		this->name = name;
		this->workdays = workdays;
		this->descendant = descendant;
		this->illness = illness;
		this->correction = correction;
		this->min=min;
		this->max = max;
		this->dtBegin = dtBegin;
		this->dtEnd = dtEnd;
		this->period = period;
		this->catId = catId;
		this->rateId = rateId;
		this->tagId = tagId;
	}

    bool isEmpty() const {
        return id == 0 && name.isEmpty();
    }

	QString name;
	bool workdays,descendant,illness,correction;
    int min,max,id;
	QDate dtBegin,dtEnd;
	BudjetPeriod period;
	int catId,rateId,tagId;
};
