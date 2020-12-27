#pragma once
#include "baselistmodel.h"
#include "unit.h"

class TagComboModel : public BaseListModel<Unit>
{
public:
    TagComboModel(QList<Unit> excludes, QObject *parent = nullptr, bool insertEmptyVal = true);
    TagComboModel(bool insertEmptyVal = true, QObject *parent = nullptr);
    int idByName(QString name);
    Unit tagById(int id) const;

private:
    void init();
};
