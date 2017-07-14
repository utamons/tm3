#include "taglistdlg.h"
#include "unitupdatedlg.h"
#include "dbhelper.h"
#include <QtGui>
#include <QDebug>
#include <QMessageBox>

TagListDlg::TagListDlg(QWidget *parent) :  QDialog(parent)
{
    setupUi(this);
    this->setWindowTitle(tr("Метки"));
    unitList->setModel(&model);
    unitList->setSelectionMode(QAbstractItemView::SingleSelection);
    selectRow(0);
}

void TagListDlg::addUnit() {
    UnitUpdateDlg<TagListModel> nameDlg;
    nameDlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if (nameDlg.exec() == QDialog::Accepted) {
        model.insert(nameDlg.getEntity());
        selectRow(model.rowCount(QModelIndex())-1);
    }
}

void TagListDlg::delUnit() {
    QModelIndex idx = unitList->currentIndex();
    if (idx.isValid()) {
        int row = idx.row();
        if (model.remove(idx) == FOREIGN_KEY_FAIL)
            QMessageBox::critical(this,tr("Ошибка"),tr("Данная единица используется в системе, удалить её нельзя!"));
        else
            selectRow(row==model.rowCount(QModelIndex())?row-1:row);
    }
}

void TagListDlg::selectRow(const int row) {
    unitList->selectionModel()->clearSelection();
    QModelIndex index = model.index(row,0);
    if(index.isValid()) {
        unitList->selectionModel()->select(index,QItemSelectionModel::Select);
        unitList->setCurrentIndex(index);
    }
}

void TagListDlg::updateUnit(QModelIndex currentIdx) {
    Unit currUnit = model.entityFromIndex(currentIdx);
    UnitUpdateDlg<TagListModel> nameDlg(currUnit);
    nameDlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if (nameDlg.exec() == QDialog::Accepted) {
        model.update(nameDlg.getEntity(),currentIdx);
    }
}
