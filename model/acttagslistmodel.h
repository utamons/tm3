#pragma once

#include "baselistmodel.h"
#include "unit.h"

class ActTagsListModel : public  BaseListModel<Unit> {
public:
    explicit ActTagsListModel(const QList<Unit> &rowList, QObject *parent = nullptr);

    void replaceList(const QList<Unit> &rowList);
    QList<Unit> getList() { return rowList; }
};
