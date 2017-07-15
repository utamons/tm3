#include "budjetupdatedlg.h"
#include "budjettablemodel.h"
#include "cattreedlg.h"
#include <QtGlobal>
#include "tm3util.h"

BudjetUpdateDlg::BudjetUpdateDlg(Budjet budjet, QWidget *parent) :
    QDialog(parent), tagModel(false) {
	setupUi(this);
	canChangeType = false;
	this->budjet = budjet;
	choiceWidth = comboChoice->width();
	initComboPeriod();
	initComboType();
	dtEnd->setDate(QDate::currentDate().addYears(1));

    txtName->setText(budjet.name);

    if (budjet.max > 0)
        txtHigh->setText(QString::number(budjet.max));
    if (budjet.min > 0)
        txtLow->setText(QString::number(budjet.min));
    if (budjet.correction)
		checkCorrection->setChecked(true);
    if (budjet.descendant)
		checkChildren->setChecked(true);
    if (budjet.illness)
		checkIllness->setChecked(true);
    if (budjet.workdays)
		checkWorkDays->setChecked(true);

    dtEnd->setDate(budjet.dtEnd);

    comboPeriod->setCurrentIndex(budjet.period);

	canChangeType = true;
    if (budjet.catId > 0) {
		typeChanged(0);
    } else if (budjet.rateId > 0) {
		comboType->setCurrentIndex(1);
    } else if (budjet.tagId > 0) {
		comboType->setCurrentIndex(2);
	} else {
        comboChoice->setModel(&catModel);
		comboChoice->setEditable(true);
        selCat = catModel.getList()[0];
        lblCat->setText(selCat.name);
		comboChoice->setCurrentIndex(0);
	}
}

void BudjetUpdateDlg::typeChanged(int idx) {
	if (canChangeType) {
		Q_ASSERT(idx > -1 && idx < 3);
		switch (idx) {

		case 0: {
			setCatMode(true);
			comboChoice->setEditable(false);
            comboChoice->setModel(&catModel);
			comboChoice->setEditable(true);

			if (budjet.catId > 0) {
                selCat = catModel.byId(budjet.catId);
                lblCat->setText(selCat.name);
                comboChoice->setCurrentIndex(comboChoice->findText(selCat.abbrev));
			} else {
                selCat = catModel.byAbbrev(comboChoice->lineEdit()->text());
                lblCat->setText(selCat.name);
                budjet.rateId = 0;
                budjet.tagId = 0;
			}
			break;
		}

		case 1: {
			setCatMode(false);
            comboChoice->setModel(&rateModel);
			if (budjet.rateId > 0) {
                QString summary = rateModel.rateById(budjet.rateId).toString();
				comboChoice->setCurrentIndex(comboChoice->findText(summary));
			} else {
                budjet.catId = 0;
                budjet.tagId = 0;
			}
			break;
		}

		case 2: {
			setCatMode(false);
            comboChoice->setModel(&tagModel);
			if (budjet.tagId > 0) {
                QString tagName = tagModel.tagById(budjet.tagId).name;
				comboChoice->setCurrentIndex(comboChoice->findText(tagName));
			} else {
                budjet.catId = 0;
                budjet.rateId = 0;
			}
			break;
		}
		}
	}
}

void BudjetUpdateDlg::choiceChanged(int) {
	int type = comboType->currentIndex();
	Q_ASSERT(type > -1 && type < 3);
	if (type == 0) {
		QLineEdit *le = comboChoice->lineEdit();
		if (le != nullptr) {
			QString s = le->text();
            selCat = catModel.byAbbrev(le->text());
            lblCat->setText(selCat.name);
        }
	}
}

void BudjetUpdateDlg::catLookup() {
	CatTreeDlg catTreeDlg;
	catTreeDlg.setModal(true);
	catTreeDlg.setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
	catTreeDlg.setRbMode();
	if (catTreeDlg.exec() == QDialog::Accepted) {
        selCat =catTreeDlg.getRbSelected();
        comboChoice->setCurrentIndex(comboChoice->findText(selCat.abbrev));
        lblCat->setText(selCat.name);
	}
}

void BudjetUpdateDlg::accept() {
	if (isEmpty(txtName->text())) {
		error(tr("Enter name!"));
	} else if (isEmpty(txtHigh->text()) && isEmpty(txtLow->text())) {
		error(tr("Enter limits!"));
	} else if (!isEmpty(txtHigh->text()) && !isNumeric(txtHigh->text())){
		error(tr("Upper limit is not a number!"));
	} else if (!isEmpty(txtLow->text()) && !isNumeric(txtLow->text())){
		error(tr("Lower limit is not a number!"));
	} else if(dtEnd->date() <= QDate::currentDate()) {
		error(tr("Last date should be after current date!"));
	} else if(BudjetTableModel::hasName(txtName->text(),budjet.id)) {
		error(tr("Budjet with given name already exists."));
	} else {
        budjet.name = txtName->text();

        budjet.correction = checkCorrection->isChecked();
        budjet.descendant = checkChildren->isChecked();
        budjet.workdays = checkWorkDays->isChecked();
        budjet.dtBegin = QDate::currentDate();
        budjet.dtEnd = dtEnd->date();
        budjet.illness = checkIllness->isChecked();
        budjet.max = txtHigh->text().toInt();
        budjet.min = txtLow->text().toInt();
        budjet.period = BudjetTableModel::periodFromInt(comboPeriod->currentIndex());

		switch (comboType->currentIndex()) {
		case 0:
            budjet.catId = selCat.id;
			break;
		case 1:
            budjet.rateId = rateModel.idBySummary(comboChoice->currentText());
			break;
		case 2:
            budjet.tagId = tagModel.idByName(comboChoice->currentText());
			break;
		}

		QDialog::accept();
	}
}

void BudjetUpdateDlg::initComboPeriod() {
	QStringList iList;

	iList.append(BudjetTableModel::period2QString(DAY));
	iList.append(BudjetTableModel::period2QString(WEEK));
	iList.append(BudjetTableModel::period2QString(MONTH));
	iList.append(BudjetTableModel::period2QString(QUARTER));
	iList.append(BudjetTableModel::period2QString(YEAR));
	iList.append(BudjetTableModel::period2QString(WORKDAY));

	comboPeriod->addItems(iList);
}

void BudjetUpdateDlg::initComboType() {
	QStringList iList;

	iList.append(tr("category"));
	iList.append(tr("rate"));
	iList.append(tr("label"));

	comboType->addItems(iList);

}
