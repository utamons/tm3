#pragma once

#include <QDialog>
#include <QCloseEvent>
#include "ui_SimpleReport.h"
#include "tm3util.h"
#include "periodhelper.h"
#include "reporthelper.h"
#include <memory>

class SimpleReportDlg: public QDialog, private Ui_SimpleReportDlg {
Q_OBJECT
public:
	explicit SimpleReportDlg(QWidget *parent = 0);

	~SimpleReportDlg() {
		emit dead();
	}

	void accept() {
		saveWinPos("SimpleReportDlg", pos(), size());
		QDialog::accept();
	}

	void showEvent(QShowEvent *) {
		makeReport();
	}

protected:
	void closeEvent(QCloseEvent *event) {
		saveWinPos("SimpleReportDlg", pos(), size());
		event->accept();
	}

	void restorePos() {
		resize(restoreWinSize("SimpleReportDlg"));
		move(restoreWinPos("SimpleReportDlg"));
	}

public slots:

	void firstDateChanged();
	void secondDateChanged();
	void periodBack();
	void periodFwd();
	void dailyReport();
	void weeklyReport();
	void monthlyReport();
	void yearReport();
	void makeReport();

	void radCatClicked(bool val);
	void radRateClicked(bool val);
	void radTagClicked(bool val);

signals:

    void dead();

private:

    std::shared_ptr<PeriodHelper> pHelper;
	ReportHelper rHelper;

};
