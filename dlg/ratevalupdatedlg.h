#pragma once

#include <QDialog>
#include "ui_RateValUpdateDlg.h"
#include "rateval.h"
#include "ratelistmodel.h"
#include "ratevaltablemodel.h"

class RateValUpdateDlg : public QDialog, private Ui_RateValUpdateDlg
{
    Q_OBJECT
public:
    explicit RateValUpdateDlg(RateVal rateVal = RateVal(), QList<RateVal> excludes = QList<RateVal>(), QWidget *parent = 0);

    void accept();

    RateVal getRateVal() {return rateVal;}

    static QList<Rate> extractRateList(QList<RateVal> rlist);

private:
    RateVal rateVal;
    RateListModel rateModel;
};
