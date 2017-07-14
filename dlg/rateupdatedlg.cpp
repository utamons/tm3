#include "rateupdatedlg.h"
#include "unitlistmodel.h"
#include "dbhelper.h"
#include <QMessageBox>

RateUpdateDlg::RateUpdateDlg(Rate rate, QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    comboUnits->setModel(&unitModel);
    this->rate = rate;
    if (!rate.isEmpty()) {
        editComment->setPlainText(rate.comment);
        editName->setText(rate.name);
        int h = rate.time/60;
        int m = rate.time%60;
        QTime t(h,m);
        editTime->setTime(t);
        for (int i=0;i<comboUnits->count();++i) {
            if (comboUnits->itemText(i) == rate.unit.name) {
                comboUnits->setCurrentIndex(i);
                break;
            }
        }
    }
}


void RateUpdateDlg::accept() {

    Unit unit = unitModel.entityFromIndex(unitModel.index(comboUnits->currentIndex(),0));
    QTime time = editTime->time();
    int mins = time.hour()*60+time.minute();
    rate.comment = editComment->toPlainText().trimmed();
    rate.name = editName->text().trimmed();
    rate.time = mins;
    rate.unit = unit;

    if (rate.name.length() == 0) {
        QMessageBox::warning(this,tr("Ошибка"),tr("Необходимо ввести название!"));
    } else if ( ! isDBUnique<RateListModel,Rate>(rate)) {
        QMessageBox::warning(this,tr("Ошибка"),tr("Такая оценка уже есть в справочнике!"));
    } else {
        QDialog::accept();
    }
}
