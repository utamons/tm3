#include "catupdatedlg.h"
#include "rateupdatedlg.h"
#include "ratevalupdatedlg.h"
#include "catlistmodel.h"
#include "tagcombodlg.h"
#include <QMessageBox>
#include "dbhelper.h"
#include "taglistmodel.h"


#define COL1_PC_SIZE 50
#define COL2_PC_SIZE 25
#define COL3_PC_SIZE 25


CatUpdateDlg::CatUpdateDlg(Category cat, QWidget *parent) :
    QDialog(parent), rateModel(cat.rates), tagModel(cat.tagList), selfId(cat.id) {
    setupUi(this);

    this->cat = cat;
    tableRate->setModel(&rateModel);
    tableRate->horizontalHeader()->setStretchLastSection(true);
    tableRate->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    tableRate->verticalHeader()->setVisible(false);
    tableRate->setSelectionBehavior(QAbstractItemView::SelectRows);

    editName->setText(cat.name);
    editAbbrev->setText(cat.abbrev);
    editComment->setPlainText(cat.comment);
    checkVirt->setChecked(cat.isVirt);
    changeVirtual();

    listViewTags->setModel(&tagModel);

    connect(&rateModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(rateModelChanged()));
    connect(&rateModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(rateModelChanged()));
    connect(&tagModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(tagModelChanged()));
    connect(&tagModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(tagModelChanged()));
    rateModelChanged();
    tagModelChanged();
}

void CatUpdateDlg::resizeColumns() {
    double tWidth = tableRate->contentsRect().width();
    tableRate->setColumnWidth(0,static_cast<int>(tWidth/100.0*COL1_PC_SIZE));
    tableRate->setColumnWidth(1,static_cast<int>(tWidth/100.0*COL2_PC_SIZE));
    tableRate->setColumnWidth(2,static_cast<int>(tWidth/100.0*COL3_PC_SIZE));
}

void CatUpdateDlg::addRate() {
    RateValUpdateDlg dlg(RateVal(),rateModel.getRowList());
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        rateModel.insert(dlg.getRateVal());
        resizeColumns();
    }
}

void CatUpdateDlg::delRate() {
    rateModel.remove(tableRate->currentIndex());
}

void CatUpdateDlg::addTag() {
    TagComboDlg dlg(tagModel.getList());
    dlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if ( dlg.exec() == QDialog::Accepted) {
        Unit tag = dlg.unit();
        if (!tag.isEmpty()) {
            tagModel.insert(tag);
        }
    }
}

void CatUpdateDlg::delTag() {
    tagModel.remove(listViewTags->currentIndex());
}

void CatUpdateDlg::updateRate(QModelIndex idx) {
    RateVal rateVal = rateModel.entityFromIndex(idx);

    QList<RateVal> excludes = rateModel.getRowList();
    excludes.removeOne(rateVal);

    RateValUpdateDlg dlg(rateVal,excludes);
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted) {
        rateModel.update(rateVal,idx);
    }
}

void CatUpdateDlg::changeVirtual() {
    const bool state = checkVirt->isChecked();
    btnAdd->setDisabled(state);
    btnDel->setDisabled(state);
    tableRate->setDisabled(state);
}

void CatUpdateDlg::accept() {
    QString name = editName->text().trimmed();
    QString abbrev = editAbbrev->text().trimmed();
    QString comment = editComment->toPlainText().trimmed();

    cat.name = name;
    cat.abbrev = abbrev;
    cat.comment = comment;
    cat.isVirt = checkVirt->isChecked();
    cat.rates = rateModel.getRowList();
    cat.tagList = tagModel.getList();

    if (name.length() == 0) {
			QMessageBox::warning(this,tr("Error"),tr("Enter name!"));
    } else if (abbrev.length() == 0) {
			QMessageBox::warning(this,tr("Error"),tr("Enter abbrev!"));
    } else if (!isDBUnique<CatListModel>(cat)) {
			QMessageBox::warning(this,tr("Error"),tr("This category already exists!"));
    } else {
        QDialog::accept();
    }
}

void CatUpdateDlg::rateModelChanged() {
    if (rateModel.rowCount(QModelIndex()) == maxDBRecords<RateListModel>())
        btnAdd->setEnabled(false);
    else
        btnAdd->setEnabled(true);
    if (rateModel.rowCount(QModelIndex()) > 0)
        btnDel->setEnabled(true);
    else
        btnDel->setEnabled(false);
}

void CatUpdateDlg::tagModelChanged() {
    if (tagModel.rowCount(QModelIndex()) ==  maxDBRecords<TagListModel>())
        btnAddTag->setEnabled(false);
    else
        btnAddTag->setEnabled(true);
    if (tagModel.rowCount(QModelIndex()) > 0)
        btnDelTag->setEnabled(true);
    else
        btnDelTag->setEnabled(false);
}
