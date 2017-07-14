#pragma once

#include <QDialog>
#include "ui_RateListDlg.h"
#include "ratelistmodel.h"

class RateListDlg : public QDialog, private Ui_rateDlg
{
    Q_OBJECT
public:
    explicit RateListDlg(QWidget *parent = 0);
    
public slots:
    void addRate();
    void delRate();
    void updateRate(QModelIndex);

private:
    RateListModel model;
    void selectRow(const int row);
    
};
