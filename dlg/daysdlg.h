#pragma once

#include <QDialog>
#include <QCloseEvent>
#include "ui_DaysDlg.h"
#include "daystablemodel.h"
#include "tm3util.h"

class DaysDlg: public QDialog, private Ui_DaysDlg {
Q_OBJECT

public:
	explicit DaysDlg(QWidget *parent = 0);

public slots:

	void monthLeft();
	void monthRight();
	void monthChanged(int);
	void setWorkday();
	void setOvertime();
	void setRestday();
	void setHolyday();
	void setVacation();
	void setIllness();
	void setCompensatory();
    void setIllnessList();

protected:
	void closeEvent(QCloseEvent *event) {
		saveWinPos("DaysDlg", pos(), size());
		event->accept();
	}

	void restorePos() {
		resize(restoreWinSize("DaysDlg"));
		move(restoreWinPos("DaysDlg"));
	}

private:
	void setType(DayType type);
    DaysTableModel model;
};
