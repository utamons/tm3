/*
 * daysdlg.cpp
 *
 *  Created on: 05 июня 2014 г.
 *      Author: oleg
 */

#include "daysdlg.h"
#include "dayshelper.h"
#include <QDebug>
#include <QDate>

DaysDlg::DaysDlg(QWidget* parent) :
		QDialog(parent) {
	setupUi(this);
	restorePos();
    tblDays->setModel(&model);
	tblDays->verticalHeader()->setVisible(false);
	tblDays->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	tblDays->horizontalHeader()->setStyleSheet("text-align: center");
	tblDays->setStyleSheet("QHeaderView::section {"
			"background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"stop:0 #616161, stop: 0.5 #505050,"
			"stop: 0.6 #434343, stop:1 #656565);"
			"color: white;"
			"padding-left: 4px;"
			"border: 1px solid #6c6c6c;"
			"} "
			"QColumnView {"
			"color: red;"

			"}");
	for (int i = 0; i < 7; ++i)
		tblDays->setColumnWidth(i, 48);

	for (int i = 0; i < 6; ++i)
		tblDays->setRowHeight(i, 47);

	QStringList months;

	months.append(tr("Январь"));
	months.append(tr("Февраль"));
	months.append(tr("Март"));
	months.append(tr("Апрель"));
	months.append(tr("Май"));
	months.append(tr("Июнь"));
	months.append(tr("Июль"));
	months.append(tr("Август"));
	months.append(tr("Сентябрь"));
	months.append(tr("Октябрь"));
	months.append(tr("Ноябрь"));
	months.append(tr("Декабрь"));

	comboMonth->insertItems(0, months);
	comboMonth->setCurrentIndex(QDate::currentDate().month() - 1);

	QAction *itemWorkday = new QAction(tr("Рабочий день"), this);
	QAction *itemOvertime = new QAction(tr("Переработка"), this);
	QAction *itemRestDay = new QAction(tr("Выходной"), this);
	QAction *itemHolyday = new QAction(tr("Праздник"), this);
	QAction *itemVacation = new QAction(tr("Отпуск"), this);
	QAction *itemIllness = new QAction(tr("Болезнь"), this);
	QAction *itemCompensatory = new QAction(tr("Отгул"), this);
    QAction *itemIllnessList = new QAction(tr("Больничный"), this);

	connect(itemWorkday, SIGNAL(triggered()), this, SLOT(setWorkday()));
	connect(itemOvertime, SIGNAL(triggered()), this, SLOT(setOvertime()));
	connect(itemRestDay, SIGNAL(triggered()), this, SLOT(setRestday()));
	connect(itemHolyday, SIGNAL(triggered()), this, SLOT(setHolyday()));
	connect(itemVacation, SIGNAL(triggered()), this, SLOT(setVacation()));
	connect(itemIllness, SIGNAL(triggered()), this, SLOT(setIllness()));
	connect(itemCompensatory, SIGNAL(triggered()), this, SLOT(setCompensatory()));
    connect(itemIllnessList, SIGNAL(triggered()), this, SLOT(setIllnessList()));

	tblDays->addAction(itemWorkday);
	tblDays->addAction(itemOvertime);
	tblDays->addAction(itemRestDay);
	tblDays->addAction(itemHolyday);
	tblDays->addAction(itemVacation);
	tblDays->addAction(itemIllness);
	tblDays->addAction(itemCompensatory);
    tblDays->addAction(itemIllnessList);

	tblDays->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void DaysDlg::monthLeft() {
	int idx = comboMonth->currentIndex();
	if (idx > 0) {
		--idx;
		comboMonth->setCurrentIndex(idx);
	}
}

void DaysDlg::monthRight() {
	int idx = comboMonth->currentIndex();
	if (idx < 11) {
		++idx;
		comboMonth->setCurrentIndex(idx);
	}
}

void DaysDlg::monthChanged(int newMonth) {
    model.init(newMonth + 1);
}

void DaysDlg::setWorkday() {
	setType(WORK);
}

void DaysDlg::setOvertime() {
	setType(OVERTIME);
}

void DaysDlg::setRestday() {
	setType(RESTDAY);
}

void DaysDlg::setHolyday() {
	setType(HOLIDAY);
}

void DaysDlg::setVacation() {
	setType(VACATION);
}

void DaysDlg::setIllness() {
	setType(ILLNESS);
}

void DaysDlg::setCompensatory() {
    setType(COMPENSATORY);
}

void DaysDlg::setIllnessList() {
    setType(ILLNESS_LIST);
}

void DaysDlg::setType(DayType type) {
	DaysHelper h;
    Day d = model.dayFromIndex(tblDays->currentIndex());
    if (!d.isEmpty()) {
		h.setType(d, type);
        model.init(d.month);
	}
}
