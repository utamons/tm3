#pragma once

#include <QObject>
#include <QDialog>
#include "ui_UnitDlg.h"
#include "unitlistmodel.h"

class UnitListDlg : public QDialog, private Ui_UnitDlg
{
    Q_OBJECT
public:
    explicit UnitListDlg(QWidget *parent = nullptr);
    
signals:
    
public slots:
    void addUnit();
    void delUnit();
    void updateUnit(QModelIndex currentIdx);
    
private:
    UnitListModel model;
    void selectRow(const int row);
};
