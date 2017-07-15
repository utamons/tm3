#include "unitlistdlg.h"
#include "unitupdatedlg.h"
#include "dbhelper.h"
#include <QtGui>
#include <QDebug>
#include <QMessageBox>

UnitListDlg::UnitListDlg(QWidget *parent) :  QDialog(parent)
{
    setupUi(this);
	this->setWindowTitle(tr("Units"));
    unitList->setModel(&model);
    unitList->setSelectionMode(QAbstractItemView::SingleSelection);
    selectRow(0);
}

void UnitListDlg::addUnit() {
    UnitUpdateDlg<UnitListModel> dlg;
    dlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if (dlg.exec() == QDialog::Accepted) {
        model.insert(dlg.getEntity());
        selectRow(model.rowCount(QModelIndex())-1);
    }
}

void UnitListDlg::delUnit() {
    QModelIndex idx = unitList->currentIndex();
    if (idx.isValid()) {
        int row = idx.row();
        if (model.remove(idx) == FOREIGN_KEY_FAIL)
			QMessageBox::critical(this,tr("Error"),tr("This unit is in use!"));
        else
            selectRow(row==model.rowCount(QModelIndex())?row-1:row);
    }
}

void UnitListDlg::selectRow(const int row) {
    unitList->selectionModel()->clearSelection();
    QModelIndex index = model.index(row,0);
    if(index.isValid()) {
        unitList->selectionModel()->select(index,QItemSelectionModel::Select);
        unitList->setCurrentIndex(index);
    }
}

void UnitListDlg::updateUnit(QModelIndex currentIdx) {
    Unit currUnit = model.entityFromIndex(currentIdx);
    UnitUpdateDlg<UnitListModel> dlg(currUnit);
    dlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if (dlg.exec() == QDialog::Accepted) {
        model.update(dlg.getEntity(),currentIdx);
    }
}
