#pragma once

#include <QDialog>
#include "ui_CatTreeDlg.h"
#include "cattreemodel.h"
#include <QDebug>
#include <QtGlobal>

class CatTreeDlg : public QDialog, private Ui_CatTreeDlg
{
    Q_OBJECT
public:
    explicit CatTreeDlg(QWidget *parent = 0);
    void showEvent(QShowEvent *) {
        expand();
    }
    void expand();
    void updateCat(QModelIndex idx);

public slots:

    void addCat();
    void addRoot();
    void delCat();

    void expand(QModelIndex idx);
    void collapse(QModelIndex idx);
    void doubleClicked(QModelIndex idx);
    void updateCat();
    void drop(QModelIndex idx);
    void setRbMode() {
        rbMode=true;
        frame_2->setVisible(false);
    }

    Category getRbSelected() const {
        Q_ASSERT (rbMode);
        return rbSelected;
    }

private:

    CatTreeModel model;
    bool restoreState;
    bool rbMode; // режим справочника
    Category rbSelected; // выбранная в режиме справочника
    
};
