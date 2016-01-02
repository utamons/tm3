#pragma once

#include <QDialog>
#include "ui_CatUpdateDlg.h"
#include "category.h"
#include "cattreemodel.h"
#include "ratevaltablemodel.h"
#include "cattagslistmodel.h"

class CatUpdateDlg : public QDialog, private Ui_CatUpdateDlg
{
    Q_OBJECT
public:
    explicit CatUpdateDlg(Category cat = Category(), QWidget *parent = 0);

    void resizeColumns();

    void showEvent(QShowEvent *) {
       resizeColumns();
    }

    Category getCat() { return cat; }

public slots:
    void addRate();
    void delRate();
    void addTag();
    void delTag();
    void updateRate(QModelIndex idx);
    void changeVirtual();
    void accept();
    void rateModelChanged();
    void tagModelChanged();


private:
    Category cat;
    RateValTableModel rateModel;
    CatTagsListModel tagModel;

    const int selfId;
};
