#pragma once

#include "baselistmodel.h"
#include "unit.h"

class CatTagsListModel :  public  BaseListModel<Unit> {
public:
    explicit CatTagsListModel(const QList<Unit> &rowList, QObject *parent=0);
    QList<Unit> getList() { return rowList; }
};
