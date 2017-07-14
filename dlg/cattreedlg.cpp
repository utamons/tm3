#include "cattreedlg.h"
#include "catupdatedlg.h"
#include <QDebug>

CatTreeDlg::CatTreeDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    restoreState = false;
    rbMode = false;

    btnAdd->setDefaultAction(actionAdd);
    btnDel->setDefaultAction(actionDel);
    btnEdit->setDefaultAction(actionEdit);
    btnAdd2Root->setDefaultAction(actionAdd2Root);
    treeView->setModel(&model);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setAutoExpandDelay(400);
    connect(&model, SIGNAL(dropped(QModelIndex)), this, SLOT(drop(QModelIndex)));
}

void CatTreeDlg::expand() {
    restoreState = true;
    auto indexes = model.match(model.index(0,0),
                               Qt::DisplayRole, "*", -1, Qt::MatchWildcard|Qt::MatchRecursive);
    for (QModelIndex index: indexes) {
        bool nodeExpanded = model.isExpanded(index);
        if (nodeExpanded && !treeView->isExpanded(index)) {
            treeView->expand(index);
        } else if (!nodeExpanded && treeView->isExpanded(index)) {
            treeView->collapse(index);
        }
    }
    restoreState = false;
}

void CatTreeDlg::addCat(){

    CatUpdateDlg dlg;
    auto currIdx = treeView->currentIndex();

    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        auto cat = dlg.getCat();
        cat.id = model.addNode(cat,currIdx);
        expand();
        treeView->selectionModel()->setCurrentIndex(model.getIndex(cat),QItemSelectionModel::ClearAndSelect);
    }
}

void CatTreeDlg::addRoot() {
    CatUpdateDlg dlg;

    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        auto cat = dlg.getCat();
        cat.id =  model.addNode(cat,QModelIndex());
        expand();
        treeView->selectionModel()->setCurrentIndex(model.getIndex(cat),QItemSelectionModel::ClearAndSelect);
    }
}

void CatTreeDlg::delCat(){
    auto selectedIndex = model.deleteNode(treeView->currentIndex());
    if (selectedIndex.isValid()) {
        expand();
        treeView->selectionModel()->setCurrentIndex(selectedIndex,QItemSelectionModel::SelectCurrent);
    }
}

void CatTreeDlg::updateCat(QModelIndex idx){
    if (idx.isValid()) {
        CatUpdateDlg dlg(model.categoryFromIndex(idx));
        dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
        if (dlg.exec() == QDialog::Accepted) {
            model.updateNode(dlg.getCat(),idx);
        }
    }
}

void CatTreeDlg::expand(QModelIndex idx) {
    if (!restoreState) {
        model.expandNode(idx,true);
    }
}

void CatTreeDlg::collapse(QModelIndex idx) {
    if (!restoreState) {
        model.expandNode(idx,false);
    }
}

void CatTreeDlg::doubleClicked(QModelIndex idx) {
    if (rbMode) {
        rbSelected = model.categoryFromIndex(idx);
        if (!rbSelected.isVirt) {
            QDialog::accept();
        }
    } else
        updateCat(idx);
}

void CatTreeDlg::updateCat() {
    updateCat(treeView->currentIndex());
}

void CatTreeDlg::drop(QModelIndex idx) {
    expand();
    if (idx.isValid())
        treeView->selectionModel()->setCurrentIndex(idx,QItemSelectionModel::ClearAndSelect);
}
