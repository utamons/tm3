#pragma once

#include "baselistmodel.h"
#include "unit.h"

class CatTagsListModel :  public  BaseListModel<Unit> {
public:
    explicit CatTagsListModel(const QList<Unit> &rowList, QObject *parent=nullptr);
    QList<Unit> getList() { return rowList; }
};
