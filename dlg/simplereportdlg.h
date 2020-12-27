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
    explicit SimpleReportDlg(QWidget *parent = nullptr);

    ~SimpleReportDlg() override {
		emit dead();
	}

    void accept() override {
		saveWinPos("SimpleReportDlg", pos(), size());
		QDialog::accept();
	}

    void showEvent(QShowEvent *) override {
		makeReport();
	}

protected:
    void closeEvent(QCloseEvent *event) override {
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
