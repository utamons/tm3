#pragma once

#include <QDialog>
#include "ui_RateUpdateDlg.h"
#include "unitlistmodel.h"
#include "ratelistmodel.h"
#include "rate.h"

class RateUpdateDlg : public QDialog, private Ui_rateUpdateDlg
{
    Q_OBJECT
public:
    explicit RateUpdateDlg(Rate rate = Rate(), QWidget *parent = nullptr);

    void accept() override;

    Rate getRate() const {
        return rate;
    }

private:
    Rate rate;
    UnitListModel unitModel;
};
