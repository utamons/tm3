#pragma once

#include "ui_BudjetUpdateDlg.h"
#include "budjet.h"
#include "catlistmodel.h"
#include "ratelistmodel.h"
#include "tagcombomodel.h"
#include "catabbrevvalidator.h"
#include <QMessageBox>

class BudjetUpdateDlg: public QDialog, private Ui_BudjetUpdateDlg {
Q_OBJECT
public:
    explicit BudjetUpdateDlg(Budjet budjet = Budjet(), QWidget *parent = 0);
    Budjet getBudjet() const {return budjet;}

public slots:

	void typeChanged(int idx);
	void choiceChanged(int idx);
	void catLookup();
	void accept();

private:
    Budjet budjet;
	void initComboPeriod();
	void initComboType();
	void error(QString msg) {
		QMessageBox::critical(this, tr("Ошибка"), msg);
	}

	inline void setCatMode(bool set) {
		if (set) {
			comboChoice->setFixedWidth(choiceWidth);
			btnLookup->setVisible(true);
			lblCat->setVisible(true);
            comboChoice->setValidator(new CatAbbrevValidator(catModel.getList(),	this));
		} else {
			btnLookup->setVisible(false);
			lblCat->setVisible(false);
			comboChoice->setFixedWidth(choiceWidth + 32);
			comboChoice->setValidator(0);
			comboChoice->setEditable(false);
		}
	}

    CatListModel catModel;
    RateListModel rateModel;
    TagComboModel tagModel;

	int choiceWidth;
	bool canChangeType;

    Category selCat;
};
