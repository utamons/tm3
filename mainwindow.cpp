#include <QtGui>
#include <QCalendarWidget>
#include "mainwindow.h"
#include "dbhelper.h"
#include "tm3util.h"
#include "unitlistdlg.h"
#include "ratelistdlg.h"
#include "cattreedlg.h"
#include "actupdatedlg.h"
#include "simplereportdlg.h"
#include <QtGlobal>
#include "taglistdlg.h"
#include "periodhelper.h"
#include "ratereporthelper.h"
#include <memory>

#define COL1_PC_SIZE 15
#define COL2_PC_SIZE 15
#define COL3_PC_SIZE 30
#define COL4_PC_SIZE 40

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent) {
    setupUi(this);
    readSettings();

    btnAdd->setDefaultAction(actionActNew);
    btnDel->setDefaultAction(actionDel);
    btnUpdate->setDefaultAction(actionUpdate);
    btnReport->setDefaultAction(actionSimpleReport);

    tableActs->setModel(&model);

    tableActs->horizontalHeader()->setStretchLastSection(true);
    tableActs->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableActs->verticalHeader()->setVisible(false);
    tableActs->setSelectionBehavior(QAbstractItemView::SelectRows);

    dateEdit->calendarWidget()->setGridVisible(true);
    dateEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    dateEdit->setDate(QDate::currentDate());

    statusbar->addPermanentWidget(&lblStatus);
    statusbar->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    statusbar->setSizeGripEnabled(false);
    statusbar->setContentsMargins(15, 0, 0, 5);


    actionActNew->setShortcut(QApplication::translate("MainWindow", restoreInsKey().toLocal8Bit().constData(), Q_NULLPTR));

    showStatus();
}

void MainWindow::resizeColumns() {
    double tWidth = tableActs->contentsRect().width();
    tableActs->setColumnWidth(0, static_cast<int>(tWidth / 100.0 * COL1_PC_SIZE));
    tableActs->setColumnWidth(1, static_cast<int>(tWidth / 100.0 * COL2_PC_SIZE));
    tableActs->setColumnWidth(2, static_cast<int>(tWidth / 100.0 * COL3_PC_SIZE));
    tableActs->setColumnWidth(3, static_cast<int>(tWidth / 100.0 * COL4_PC_SIZE));
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWinPos("", pos(), size());
    event->accept();
}

void MainWindow::actUnitsDlg() {
    UnitListDlg unitDlg;

    unitDlg.setModal(true);
    unitDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    unitDlg.exec();
}

void MainWindow::actRates() {
    RateListDlg rateListDlg;
    rateListDlg.setModal(true);
    rateListDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    rateListDlg.exec();
}

void MainWindow::actCatDlg() {
    CatTreeDlg catTreeDlg;
    catTreeDlg.setModal(true);
    catTreeDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    catTreeDlg.exec();
}

void MainWindow::addAct() {
    ActUpdateDlg dlg;
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        auto split = splitByDay(dlg.getAct());
        model.insert(split.first);
        if (!split.second.isEmpty())
            model.insert(split.second);
        selectLastRow();
    }
    showStatus();
    fillLoadLabel();
}

void MainWindow::addAct2() {
    addAct();
}

void MainWindow::updateAct(QModelIndex idx) {
    Activity act = model.entityFromIndex(idx);
    Q_ASSERT(!act.isEmpty());
    ActUpdateDlg dlg(act,  act.time);
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        auto split = splitByDay(dlg.getAct());
        model.update(split.first,idx);
        if (!split.second.isEmpty())
            model.insert(split.second);

        tableActs->selectionModel()->select(QItemSelection(model.index(idx.row(), 0), model.index(idx.row(), 3)),
                                            QItemSelectionModel::SelectCurrent);
        tableActs->setCurrentIndex(idx);
    }
    showStatus();
    fillLoadLabel();
}

void MainWindow::updateAction() {
    updateAct(tableActs->currentIndex());
}

void MainWindow::delAct() {
    int row = tableActs->currentIndex().row();
    if (row > -1) {
        Activity act = model.entityFromIndex(tableActs->currentIndex());


        if (askUser(tr("The record will be deleted. Are you sure?"),
                    act.cat.name + tr(" от ") + act.time.toString("dd.MM.yy hh:mm")) == QMessageBox::Yes) {

            model.remove(tableActs->currentIndex());
            int rowCount = model.rowCount();
            --row;

            if (rowCount > 0) {

                if (row < 0) row = 0;

                tableActs->selectionModel()->select(QItemSelection(model.index(row, 0), model.index(row, 3)),
                                                    QItemSelectionModel::SelectCurrent);
                tableActs->setCurrentIndex(model.index(row, 0));
            }

            showStatus();
        }
    }
}

void MainWindow::about() {
    QString msg = tr("<b>TM3</b> Personal timetracker.<br/><br/> ") +

            tr("Version ") + PROGRAM_VERSION + "<br/>Copyleft (c) " + getCopyrightYears() + ", CornKnight.";

    QMessageBox::about(this, tr("About"), msg);
}

void MainWindow::readSettings() {
    resize(restoreWinSize(""));
    move(restoreWinPos(""));
}

void MainWindow::dateChanged() {
    model.init(dateEdit->date().startOfDay(), dateEdit->date().addDays(1).startOfDay());
    tableActs->reset();
    selectLastRow();
    fillLoadLabel();
}

void MainWindow::dayBack() {
    dateEdit->setDate(dateEdit->date().addDays(-1));
}

void MainWindow::dayFwd() {
    dateEdit->setDate(dateEdit->date().addDays(1));
}

void MainWindow::showStatus() {
    QSqlQuery q(getDb());
    QString status;
    q.prepare("select count(id) as cnt from actions");
    execQuery(q, [&q,&status]() {
        int cnt = field<int>(q,"cnt");
        status.append(tr("Overall records: ")+QString::number(cnt));
    });
    QDateTime last = model.getLastTime();

    QString insKey = restoreInsKey();

    lblStatus.setText(status + tr("  Last: ") + last.toString("hh:mm")+" ("+insKey+")");
}

void MainWindow::actSimpleReport() {
    SimpleReportDlg dlg;
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    dlg.exec();
}

void MainWindow::today() {
    dateEdit->setDate(QDate::currentDate());
}


/*
    I use two different keyboards and two different keys are convinient for insert action on these keyboards.
*/
void MainWindow::toggleInsKey(){
    QString lastKey = restoreInsKey();
    QString newKey;

    if (lastKey == "Del") {
        newKey = "Backspace";
    } else {
        newKey = "Del";
    }

    actionActNew->setShortcut(QApplication::translate("MainWindow", newKey.toLocal8Bit().constData(), Q_NULLPTR));
    saveInsKey(newKey);
    showStatus();
}

void MainWindow::actSetDark() {
    setDark();
}

void MainWindow::actTagDlg() {
    TagListDlg tagDlg;
    tagDlg.setModal(true);
    tagDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    tagDlg.exec();
}

QString MainWindow::getLoadStr(double load) {
    const QString GREEN = "#00aa00";
    const QString RED = "#dd0000";
    const QString ORANGE="#fa7e25";
    const QString BOLD="<span style='font-size:10pt; font-weight:600; color:";

    QString strsum = QString::number(load,'f',2);

    QString result;

    if (load > 12) {
        result = BOLD+RED+"'>N-load: "+strsum+"</span>";
    } else if (load > 10){
        result = BOLD+ORANGE+"'>N-load: "+strsum+"</span>";
    } else if (load > 0) {
        result = BOLD+GREEN+"'>N-load: "+strsum+"</span>";
    } else {
        result = "N-load: "+strsum;
    }

    return result;
}

void MainWindow::fillLoadLabel() {
    RateReportHelper rh(std::make_shared<PeriodHelper> (dateEdit, dateEdit));
    lblLoad->setText(getLoadStr(rh.getRateSum(2)));
}



std::pair<Activity, Activity> MainWindow::splitByDay(Activity act) {
    std::pair<Activity, Activity> result;

    QDate nextDate = act.time.addSecs(act.mins * 60).date();
    if (act.time.date() != nextDate) {
        QDateTime nextTime = nextDate.startOfDay();
        int minsTo = static_cast<int>(act.time.secsTo(nextTime) / 60);
        Q_ASSERT(minsTo > 0);

        int minsAfter = act.mins - minsTo;

        if (minsAfter > 0) {
            Activity act2 = act;
            act2.time = nextTime;
            act2.mins = minsAfter;
            act.mins = minsTo;
            result.second = act2;
        }
    }

    result.first = act;
    return result;
}

void MainWindow::selectLastRow() {
    tableActs->selectionModel()->select(QItemSelection(model.lastIndex(0), model.lastIndex(3)),
                                        QItemSelectionModel::SelectCurrent);
    tableActs->scrollToBottom();
    tableActs->setFocus();
    tableActs->setCurrentIndex(model.lastIndex(0));
}
