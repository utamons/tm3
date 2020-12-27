#pragma once

#include <QList>
#include <QDateTime>
#include "category.h"
#include "entity.h"
#include <QDebug>
#include "tm3util.h"

class Activity : public Entity {
public:
    Activity();

    bool isEmpty() const override;

    QList<QVariant> toTableRow() const override;

    void setId(QVariant id) override;

    QList<RateVal> rateValList;
    QList<Unit> tagList;
    QDateTime time;
    Category cat;
    int mins;
    int id;
    QString comment;
};








