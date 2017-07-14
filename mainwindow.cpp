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
#include "budjetlistdlg.h"
#include "taglistdlg.h"
#include "periodhelper.h"
#include "ratereporthelper.h"
#include <memory>

// Размеры колонок таблицы в процентах:
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

    tableActs->setStyleSheet("QHeaderView::section {"
                             "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                             "stop:0 #616161, stop: 0.5 #505050,"
                             "stop: 0.6 #434343, stop:1 #656565);"
                             "color: white;"
                             "padding-left: 4px;"
                             "border: 1px solid #6c6c6c;"
                             "text-align:left;"
                             "}");
    tableActs->horizontalHeader()->setStretchLastSection(true);
    tableActs->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableActs->horizontalHeader()->setStyleSheet("text-align: left"); // прикольно но без этого не работает предыдущий стиль
    tableActs->verticalHeader()->setVisible(false);
    tableActs->setSelectionBehavior(QAbstractItemView::SelectRows);

    dateEdit->calendarWidget()->setGridVisible(true);
    dateEdit->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    dateEdit->setDate(QDate::currentDate());

    statusbar->addPermanentWidget(&lblStatus);
    statusbar->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    statusbar->setSizeGripEnabled(false);
    statusbar->setContentsMargins(15, 0, 0, 5);

    showStatus();
}

void MainWindow::resizeColumns() {
    double tWidth = tableActs->contentsRect().width();
    tableActs->setColumnWidth(0, tWidth / 100.0 * COL1_PC_SIZE);
    tableActs->setColumnWidth(1, tWidth / 100.0 * COL2_PC_SIZE);
    tableActs->setColumnWidth(2, tWidth / 100.0 * COL3_PC_SIZE);
    tableActs->setColumnWidth(3, tWidth / 100.0 * COL4_PC_SIZE);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveWinPos("", pos(), size());
    event->accept();
}

void MainWindow::actUnitsDlg() {
    UnitListDlg unitDlg;

    unitDlg.setModal(true);
    /*
     Почему-то никак не мог найти комбинацию флагов, которая
     отключает лишние значки в KDE. Может с Windows оно работает.
     Ниженайденная комбинация хотя бы убирает значок "?"
     */
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
    QString msg = tr("<b>TM3</b> Persona timetracker.<br/><br/> ") +

            tr("Version ") + PROGRAM_VERSION + "<br/>Copyright (c) " + getCopyrightYears() + ", CornKnight.";

    QMessageBox::about(this, tr("About"), msg);
}

void MainWindow::readSettings() {
    resize(restoreWinSize(""));
    move(restoreWinPos(""));
}

void MainWindow::dateChanged() {
    model.init(QDateTime(dateEdit->date()), QDateTime(dateEdit->date().addDays(1)));
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
    execQuery(q, [q,&status,this]() {
        int cnt = getField<int>(q,"cnt");
        status.append(tr("Overall records: ")+QString::number(cnt));
    });
    QDateTime last = model.getLastTime();
    lblStatus.setText(status + tr("  Last: ") + last.toString("hh:mm"));
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

void MainWindow::actTagDlg() {
    TagListDlg tagDlg;
    tagDlg.setModal(true);
    tagDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    tagDlg.exec();
}

QString MainWindow::getLoadStr(double load) {
    const QString RED = "#dd0000";
    const QString ORANGE="#fa7e25";
    const QString BOLD="<span style='font-size:10pt; font-weight:600; color:";

    QString strsum = QString::number(load,'f',2);

    QString result;

    if (load > 6 && load < 7.5) {
        result = BOLD+ORANGE+"'>N-load: "+strsum+"</span>";
    } else if (load > 7.5){
        result = BOLD+RED+"'>N-load: "+strsum+"</span>";
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
        QDateTime nextTime(nextDate);
        int minsTo = act.time.secsTo(nextTime) / 60;
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

void MainWindow::actBudjetDlg() {
    BudjetListDlg budjetDlg(this);
    budjetDlg.setModal(true);
    budjetDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    budjetDlg.exec();
}
