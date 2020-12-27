#include <QDebug>
#include <QCompleter>
#include <QMessageBox>

#include "actupdatedlg.h"
#include "tm3util.h"
#include "catabbrevvalidator.h"
#include "cattreedlg.h"
#include "ratevalupdatedlg.h"
#include "tagcombodlg.h"
#include "taglistmodel.h"

#define COL1_PC_SIZE 50
#define COL2_PC_SIZE 25
#define COL3_PC_SIZE 25

ActUpdateDlg::ActUpdateDlg(Activity act, QDateTime beginTime, QWidget *parent) :
        QDialog(parent), rateModel(act.rateValList), tagModel(act.tagList) {
	setupUi(this);

    this->act = act;

	// Keys bidnigs to actionCloseAll
	QList<QKeySequence> rets;
	rets.append(QKeySequence("Enter"));
	rets.append(QKeySequence("Return"));
	actionCloseAll->setShortcuts(rets);

	// The form will close on pressing Enter on these elements
	comboCats->addAction(actionCloseAll);
	spinHour->addAction(actionCloseAll);
	spinMin->addAction(actionCloseAll);
	spinMin->addAction(actionCloseAll);
	editDateTime->addAction(actionCloseAll);
	editComment->addAction(actionCloseAll);

    tableRates->setModel(&rateModel);
	tableRates->horizontalHeader()->setStretchLastSection(true);
	tableRates->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
	tableRates->verticalHeader()->setVisible(false);
	tableRates->setSelectionBehavior(QAbstractItemView::SelectRows);

    if (this->act.id == 0)
        editDateTime->setDateTime(roundTime(ActivityTableModel::getLastTime()));
	else
        editDateTime->setDateTime(act.time);

	// if this is the last record, and less than 24 hrs passed.
    if (beginTime.addSecs(act.mins * 60) == ActivityTableModel::getLastTime()
            && beginTime.addSecs(24 * 3600) >= QDateTime::currentDateTime()
            && beginTime.secsTo(QDateTime::currentDateTime()) > 0) {
		spinHour->setValue(
                static_cast<int>(beginTime.secsTo(QDateTime::currentDateTime()) / 3600));
		spinMin->setValue(
				roundMins(
						beginTime.secsTo(QDateTime::currentDateTime()) % 3600
								/ 60));
	} else {
        spinHour->setValue(act.mins / 60);
        spinMin->setValue(act.mins % 60);
	}

	// Set category

	comboCats->setEditable(false);
    comboCats->setModel(&catModel);
    comboCats->setEditable(true);
    comboCats->setValidator(new CatAbbrevValidator(catModel.getList(), this));
    if (this->act.cat.abbrev.length() > 0) {
        comboCats->setCurrentIndex(comboCats->findText(this->act.cat.abbrev));
	} else {
        this->act.cat =  catModel.byAbbrev(comboCats->lineEdit()->text());
        rateModel.replaceList(this->act.cat.rates);
        tagModel.replaceList(this->act.cat.tagList);
	}
    labelCat->setText(this->act.cat.name);
	
    listViewTags->setModel(&tagModel);

    editComment->setPlainText(this->act.comment);

    connect(&rateModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(rateModelChanged()));
    connect(&rateModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(rateModelChanged()));
    connect(&tagModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(tagModelChanged()));
    connect(&tagModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(tagModelChanged()));
    rateModelChanged();
    tagModelChanged();
}

void ActUpdateDlg::timeChanged(const QTime& time) {
	if (time.minute() % 5 != 0)
        editDateTime->setDateTime(roundTime(editDateTime->dateTime()));
}

void ActUpdateDlg::addTag() {
    TagComboDlg dlg(tagModel.getList());
    dlg.setWindowFlags(Qt::Dialog|Qt::CustomizeWindowHint);
    if ( dlg.exec() == QDialog::Accepted) {
        Unit tag = dlg.unit();
        if (!tag.isEmpty()) {
            tagModel.insert(tag);
        }
    }
}

void ActUpdateDlg::delTag() {
    tagModel.remove(listViewTags->currentIndex());
}

void ActUpdateDlg::minsEditFinished() {
	spinMin->setValue(roundMins(spinMin->value()));
}

void ActUpdateDlg::enterPressed() {
    accept();
}

void ActUpdateDlg::rateModelChanged() {
    if (rateModel.rowCount(QModelIndex()) == maxDBRecords<RateListModel>())
        btnAddRate->setEnabled(false);
     else
        btnAddRate->setEnabled(true);
    if (rateModel.rowCount(QModelIndex()) > 0)
        btnDelRate->setEnabled(true);
    else
        btnDelRate->setEnabled(false);
}

void ActUpdateDlg::tagModelChanged() {
    if (tagModel.rowCount(QModelIndex()) == maxDBRecords<TagListModel>())
        btnAddTag->setEnabled(false);
     else
        btnAddTag->setEnabled(true);
    if (tagModel.rowCount(QModelIndex()) > 0)
        btnDelTag->setEnabled(true);
    else
        btnDelTag->setEnabled(false);
}

void ActUpdateDlg::resizeColumns() {
	double tWidth = tableRates->contentsRect().width();
    tableRates->setColumnWidth(0, static_cast<int>(tWidth / 100.0 * COL1_PC_SIZE));
    tableRates->setColumnWidth(1, static_cast<int>(tWidth / 100.0 * COL2_PC_SIZE));
    tableRates->setColumnWidth(2, static_cast<int>(tWidth / 100.0 * COL3_PC_SIZE));
}

void ActUpdateDlg::accept() {
	int mins = spinHour->value() * 60 + spinMin->value();

	if (mins == 0) {
		QMessageBox::warning(this, tr("Error"),
		        tr("Enter time duration!"));
	} else if (comboCats->lineEdit()->text().length() == 0) {
		QMessageBox::warning(this, tr("Error"),
		        tr("Please choose category!"));
    } else if (ActivityTableModel::isCrossTime(editDateTime->dateTime(), mins, act.id)) {
		QMessageBox::warning(this, tr("Error"),
		        tr("This action time overlaps other action!"));
	} else {
        act.comment = editComment->toPlainText();
        act.time = editDateTime->dateTime();
        act.mins = mins;
        act.rateValList = rateModel.getRowList();
        act.tagList = tagModel.getList();
		QDialog::accept();
	}
}

void ActUpdateDlg::catFieldEdited() {
    QString text = comboCats->lineEdit()->text();
    comboCats->lineEdit()->setText(text.toUpper());
    Category selCat = catModel.byAbbrev(comboCats->lineEdit()->text());
    if ( selCat.id != act.cat.id && !selCat.isEmpty()) {
		int enteredLen = comboCats->lineEdit()->text().size();

        comboCats->lineEdit()->setText(selCat.abbrev);
        comboCats->lineEdit()->setSelection(enteredLen,selCat.abbrev.length());

        labelCat->setText(selCat.name);
        rateModel.replaceList(selCat.rates);
        tagModel.replaceList(selCat.tagList);
        act.cat = selCat;
    } else if (selCat.isEmpty()) {
		labelCat->setText("");
	} else {
        labelCat->setText(act.cat.name);
	}
}

void ActUpdateDlg::addRate() {
    RateValUpdateDlg dlg(RateVal(),rateModel.getRowList());
	dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted)
        rateModel.insert(dlg.getRateVal());
}

void ActUpdateDlg::delRate() {
    rateModel.remove(tableRates->currentIndex());
}

void ActUpdateDlg::updateRate(QModelIndex idx) {
    RateVal rateVal = rateModel.entityFromIndex(idx);

    QList<RateVal> excludes = rateModel.getRowList();
    excludes.removeOne(rateVal);

    RateValUpdateDlg dlg(rateVal,excludes);
	dlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    if (dlg.exec() == QDialog::Accepted)
        rateModel.update(dlg.getRateVal(),idx);
}

void ActUpdateDlg::showCatTree() {
	CatTreeDlg catTreeDlg;
	catTreeDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	catTreeDlg.setRbMode();
	if (catTreeDlg.exec() == QDialog::Accepted) {
        Category selCat = catTreeDlg.getRbSelected();
        comboCats->setCurrentIndex(comboCats->findText(selCat.abbrev));
	}
}

