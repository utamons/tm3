#include "budjetlistdlg.h"
#include "budjetupdatedlg.h"
#include <qdebug.h>
#include <QMessageBox>

BudjetListDlg::BudjetListDlg(QWidget *parent) :
		QDialog(parent) {
	setupUi(this);
    tblBudjets->setModel(&model);

	tblBudjets->setStyleSheet("QHeaderView::section {"
			"background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"stop:0 #616161, stop: 0.5 #505050,"
			"stop: 0.6 #434343, stop:1 #656565);"
			"color: white;"
			"padding-left: 4px;"
			"border: 1px solid #6c6c6c;"
			"text-align:left;"
			"}");
	tblBudjets->horizontalHeader()->setStretchLastSection(true);
	tblBudjets->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	tblBudjets->horizontalHeader()->setStyleSheet("text-align: left");
	tblBudjets->verticalHeader()->setVisible(false);
	tblBudjets->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void BudjetListDlg::add() {
    BudjetUpdateDlg dlg;
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        model.addRow(dlg.getBudjet());
        selectRow(model.rowCount(QModelIndex()) - 1);
	}
}

void BudjetListDlg::del() {
	QModelIndex idx = tblBudjets->currentIndex();
	if (idx.isValid()) {
		int row = idx.row();
		if (model.deleteRow(idx) == FOREIGN_KEY_FAIL)
			QMessageBox::critical(this, QObject::tr("Error"), "There are reports, based on this budget!");
		else
			selectRow(row==model.rowCount(QModelIndex())?row-1:row);
	}
}

void BudjetListDlg::edit(QModelIndex idx) {
     Budjet b = model.budjetFromIndex(idx);
     BudjetUpdateDlg dlg(b);
     dlg.setModal(true);
     dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
     if (dlg.exec() == QDialog::Accepted) {
         model.updateRow(dlg.getBudjet(),idx);
         selectRow(model.rowCount(QModelIndex()) - 1);
	 }
}

void BudjetListDlg::showDay() {
}

void BudjetListDlg::showWeek() {
}

void BudjetListDlg::showMonth() {
}

void BudjetListDlg::showQuarter() {
}

void BudjetListDlg::showYear() {
}

void BudjetListDlg::showAll() {
}

void BudjetListDlg::showOld() {
}

void BudjetListDlg::selectRow(int row) {
	tblBudjets->selectionModel()->clearSelection();
    QModelIndex index = model.index(row, 0);
	if (index.isValid()) {
		tblBudjets->selectionModel()->select(index, QItemSelectionModel::Select);
		tblBudjets->setCurrentIndex(index);
	}
}
