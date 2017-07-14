#include "simplereportdlg.h"
#include <QCalendarWidget>
#include "tm3util.h"
#include "dbhelper.h"
#include "catreporthelper.h"
#include "tagreporthelper.h"
#include "ratereporthelper.h"
#include "budjetreporthelper.h"

SimpleReportDlg::SimpleReportDlg(QWidget *parent) :
	QDialog(parent) {
	setupUi(this);
	restorePos();

    pHelper = std::make_shared<PeriodHelper> (dtFirst, dtSecond);

	dtSecond->setDate(QDate::currentDate());
	dtFirst->setDate(QDate::currentDate());
	dtFirst->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
	dtSecond->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    connect(pHelper.get(), SIGNAL(makeReport()), this, SLOT(makeReport()));
    connect(this,SIGNAL(dead()),pHelper.get(),SLOT(parentDead()));


	rootCatList.append(tr("A"));
	rootCatList.append(tr("B"));
	rootCatList.append(tr("C"));

}

void SimpleReportDlg::firstDateChanged() {
	pHelper->dt1Change();
}

void SimpleReportDlg::secondDateChanged() {
	pHelper->dt2Change();
}

void SimpleReportDlg::periodBack() {
	pHelper->back();
}

void SimpleReportDlg::periodFwd() {
	pHelper->fwd();
}

void SimpleReportDlg::dailyReport() {
	pHelper->day();
}

void SimpleReportDlg::weeklyReport() {
	pHelper->week();
}

void SimpleReportDlg::monthlyReport() {
	pHelper->month();
}

void SimpleReportDlg::yearReport() {
	pHelper->year();
}

void SimpleReportDlg::makeReport() {
	rHelper.clear();
	editSummary->clear();

	if (radCat->isChecked()) {
       CatReportHelper cHelper(pHelper);
        rHelper.appendHeader(tr("Report of ").append(pHelper->getName()));
		rHelper.appendParagraph(
					tr("Overall: ").append(
						timeFromMins(pHelper->periodMins())));
		rHelper.appendParagraph(tr("Measured: ").append(cHelper.getTotalTime()));

        for(QString cat : rootCatList){
            rHelper.appendValueTable(cat,cHelper.getRootCatReport(cat));
		}

	} else if (radTag->isChecked()) {
		rHelper.appendHeader(tr("Report of ").append(pHelper->getName()));
		rHelper.appendParagraph("");
       TagReportHelper tHelper(pHelper);
        auto tagRep = tHelper.getTagsReport();
		if (tagRep.length() > 0) {
			rHelper.appendValueTable(tr("Tags"),tagRep);
		}

	} else if (radBudjet->isChecked()) {
        BudjetReportHelper bHelper(pHelper);
        auto catRep = bHelper.getCatBudjetReport();
		if (catRep.length() > 0) {
			rHelper.appendValueTable(tr("Бюджеты по категориям"),catRep);
		}

        auto rateRep = bHelper.getRateBudjetReport();
		if (rateRep.length() > 0) {
			rHelper.appendValueTable(tr("Бюджеты по оценкам"),rateRep);
		}

        auto tagRep = bHelper.getTagBudjetReport();
		if (tagRep.length() > 0) {
			rHelper.appendValueTable(tr("Бюджеты по меткам"),tagRep);
		}

	} else if (radRate->isChecked()) {
		rHelper.appendHeader(tr("Report of ").append(pHelper->getName()));
		rHelper.appendParagraph("");
        RateReportHelper rateHelper(pHelper);
        auto rateRep = rateHelper.getRatesReport();
		if (rateRep.length() > 0) {
			rHelper.appendValueTable(tr("Rates"),rateRep);
		}
		}

	editSummary->setHtml(rHelper.getHTML());
	editSummary->setReadOnly(true);
}

void SimpleReportDlg::radCatClicked(bool) {
	makeReport();
}

void SimpleReportDlg::radRateClicked(bool) {
	makeReport();
}

void SimpleReportDlg::radTagClicked(bool) {
	makeReport();
}

void SimpleReportDlg::radBudjetClicked(bool) {
    makeReport();
}

