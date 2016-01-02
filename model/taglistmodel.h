#pragma once
#include "baselistmodel.h"
#include "unit.h"
#include "dbhelper.h"

class TagListModel: public BaseListModel<Unit>
{
    Q_OBJECT
public:
    explicit TagListModel(QObject *parent = 0);

private:

    static void prepareDBUnique(QSqlQuery &q, Unit tag);
    static void prepareDBMaxRecords(QSqlQuery &q);

    friend class BaseListModel<Unit>;
    friend bool isDBUnique<TagListModel,Unit>(Unit entity);
    friend int maxDBRecords<TagListModel>();
};
