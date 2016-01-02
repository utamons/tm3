#include "ratevalupdatedlg.h"
#include <QMessageBox>
#include "unitlistmodel.h"

RateValUpdateDlg::RateValUpdateDlg(RateVal rateval, QList<RateVal> excludes, QWidget *parent) :
    rateModel(extractRateList(excludes)), QDialog(parent) {

    setupUi(this);

    this->rateVal = rateval;


    comboRates->setModel(&rateModel);

    editValue->setText(QString::number(rateVal.value));

    if (!rateVal.isEmpty()) {
        for (int i=0;i<comboRates->count();++i) {
            if (comboRates->itemText(i) == rateVal.rate.toString()) {
                comboRates->setCurrentIndex(i);
                break;
            }
        }
    }
}



void RateValUpdateDlg::accept() {
    Rate rate = rateModel.entityFromIndex(rateModel.index(comboRates->currentIndex(),0));
    if (editValue->text().trimmed().length() == 0) {
        QMessageBox::warning(this,tr("Ошибка"),tr("Необходимо ввести значение!"));
    } else {
        rateVal.rate = rate;
        rateVal.value = editValue->text().trimmed().toInt();
        QDialog::accept();
    }
}

QList<Rate> RateValUpdateDlg::extractRateList(QList<RateVal> rlist) {
    QList<Rate> result;
    for (RateVal r:rlist) {
        result.append(r.rate);
    }
    return result;
}
