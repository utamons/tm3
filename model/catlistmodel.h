#pragma once

#include "baselistmodel.h"
#include "category.h"

/*
 * Category list for drop-down lists and so on
 */
class CatListModel : public BaseListModel<Category>
{
public:
    explicit CatListModel(QObject *parent = 0);

    QList<Category> getList() const { return rowList; }

    Category byAbbrev(QString abbrev);

    Category byId(long id);

private:

    static void prepareDBUnique(QSqlQuery &q, Category cat);

    friend class BaseListModel<Category>;
    friend bool isDBUnique<CatListModel,Category>(Category entity);
};
