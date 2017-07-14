#include "tagcombodlg.h"

TagComboDlg::TagComboDlg(QList<Unit> excludes, QWidget *parent) : QDialog(parent) {
    setupUi(this);
    retranslateUi(this);
    model = new TagComboModel(excludes,this,false);
    if (model->rowCount() == 0)
        comboTag->setEnabled(false);
    else
        comboTag->setModel(model);
}

void TagComboDlg::accept() {
    if (model->rowCount() > 0) {
        selUnit = model->entityFromIndex(model->index(comboTag->currentIndex(),0));
    }
    QDialog::accept();
}


