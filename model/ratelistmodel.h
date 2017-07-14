#pragma once

#include "baselistmodel.h"
#include <QList>
#include "rate.h"

class RateListModel : public BaseListModel<Rate>
{
    Q_OBJECT
public:
    explicit RateListModel(QList<Rate> excludes = QList<Rate>(), QObject *parent = 0);

	int idBySummary(QString name) const;

    Rate rateById(int id) const;

private:

    static void prepareDBUnique(QSqlQuery &q, Rate rate);
    static void prepareDBMaxRecords(QSqlQuery &q);

    friend class BaseListModel<Rate>;
    friend bool isDBUnique<RateListModel,Rate>(Rate entity);
    friend int maxDBRecords<RateListModel>();
};
