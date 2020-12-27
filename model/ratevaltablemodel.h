#pragma once

#include "basetablemodel.h"
#include "rateval.h"
#include "category.h"
#include "activity.h"
#include <QList>

class RateValTableModel : public BaseTableModel<RateVal>
{
    Q_OBJECT
public:
    RateValTableModel(QList<RateVal> rowList, QObject *parent = nullptr);

    QList<RateVal> getRowList() const { return rowList; }
    void replaceList(const QList<RateVal> &newList);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
};
