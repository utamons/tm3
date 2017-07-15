#include "ratelistdlg.h"
#include "rateupdatedlg.h"
#include <QtGlobal>
#include "dbhelper.h"
#include <QMessageBox>

RateListDlg::RateListDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    retranslateUi(this);
    listRates->setModel(&model);
    listRates->setSelectionMode(QAbstractItemView::SingleSelection);
    selectRow(0);
}

void RateListDlg::selectRow(const int row) {
    listRates->selectionModel()->clearSelection();
    QModelIndex index = model.index(row,0);
    if(index.isValid()) {
        listRates->selectionModel()->select(index,QItemSelectionModel::Select);
        listRates->setCurrentIndex(index);
    }
}

void RateListDlg::addRate(){
    RateUpdateDlg updateDlg;
    updateDlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if (updateDlg.exec() == QDialog::Accepted) {
        Rate rate = updateDlg.getRate();
        model.insert(rate);
        selectRow(model.rowCount(QModelIndex())-1);
    }
}

void RateListDlg::delRate(){
    QModelIndex idx = listRates->currentIndex();
    if (idx.isValid()) {
        int row = idx.row();
        if (model.remove(idx) == FOREIGN_KEY_FAIL)
			QMessageBox::critical(this,tr("Error"),tr("The rate is in use, you cannot remove this!"));
        else
            selectRow(row==model.rowCount(QModelIndex())?row-1:row);
    }
}

void RateListDlg::updateRate(QModelIndex x){
    Q_ASSERT (x.isValid());
    Rate rate = model.entityFromIndex(x);
    RateUpdateDlg updateDlg(rate);
    updateDlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if (updateDlg.exec() == QDialog::Accepted) {
        model.update(updateDlg.getRate(),x);
        selectRow(model.rowCount(QModelIndex())-1);
    }
}
