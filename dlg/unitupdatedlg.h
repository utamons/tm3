#pragma once

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include "ui_UnitNameDlg.h"
#include "unitlistmodel.h"

template <typename T>
class UnitUpdateDlg : public QDialog, private Ui_UnitNameDlg
{
public:

    Unit getEntity() { return unit; }

UnitUpdateDlg(Unit unit= Unit(), QWidget *parent=nullptr) :  QDialog(parent) {
    setupUi(this);
    this->unit = unit;
    if (!unit.isEmpty())
        nameLineEdit->setText(unit.name);
}

void accept() {
    unit.name = nameLineEdit->text().trimmed();
    if (unit.name.length() == 0) {
       QMessageBox::warning(this,tr("Ошибка"),tr("Необходимо ввести название!"));
    } else if (!isDBUnique<T>(unit)) {
        QMessageBox::warning(this,tr("Ошибка"),tr("Такая единица уже есть в справочнике!"));
    } else {
        QDialog::accept();
    }
}

void showEvent(QShowEvent *event) {
    nameLineEdit->setFocus();
    nameLineEdit->selectAll();
    event->accept();
}

private:
    Unit unit;
};
