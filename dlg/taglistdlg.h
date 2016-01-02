#pragma once

#include <QObject>
#include <QDialog>
#include "ui_UnitDlg.h"
#include "taglistmodel.h"

class TagListDlg : public QDialog, private Ui_UnitDlg
{
    Q_OBJECT
public:
    explicit TagListDlg(QWidget *parent = 0);

signals:

public slots:
    void addUnit();
    void delUnit();
    void updateUnit(QModelIndex currentIdx);

private:
    TagListModel model;
    void selectRow(const int row);
};
