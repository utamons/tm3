#pragma once

#include "baselistmodel.h"
#include "unit.h"
#include "dbhelper.h"

class UnitListModel: public BaseListModel<Unit>
{
    Q_OBJECT
public:
    explicit UnitListModel(QObject *parent = nullptr);

private:

    static void prepareDBUnique(QSqlQuery &q, Unit tag);
    static void prepareDBMaxRecords(QSqlQuery &q);

    friend class BaseListModel<Unit>;
    friend bool isDBUnique<UnitListModel,Unit>(Unit entity);
    friend int maxDBRecords<UnitListModel>();
};
