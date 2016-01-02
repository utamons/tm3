#pragma once

#include <QDialog>
#include "ui_BudjetListDlg.h"
#include "budjettablemodel.h"

class BudjetListDlg : public QDialog, private Ui_BudjetListDlg {
    Q_OBJECT
public:
    explicit BudjetListDlg(QWidget *parent = 0);
    
signals:
    
public slots:

void add();
void del();
void edit(QModelIndex);
void showDay();
void showWeek();
void showMonth();
void showQuarter();
void showYear();
void showAll();
void showOld();
void selectRow(int row);


private:

BudjetTableModel model;
    
};
