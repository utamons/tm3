#pragma once

#include <QDialog>
#include "ui_TagComboDlg.h"
#include "unit.h"
#include "tagcombomodel.h"

class TagComboDlg : public QDialog, private Ui_TagComboDlg
{
public:
    TagComboDlg(QList<Unit> excludes=QList<Unit>(),QWidget *parent=nullptr);
    void accept() override;
    Unit unit() const {return selUnit;}

private:
    TagComboModel *model;
    Unit selUnit;

};
