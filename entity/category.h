#pragma once

#include <QList>
#include <QString>
#include "rateval.h"
#include "entity.h"
#include <QDebug>

class Category : public Entity {
public:
    Category();

    QString toString() const override;

    bool isEmpty() const override;

    void setId (QVariant id) override;

    int id;
    QString name;
    QString comment;
    QString abbrev;
    bool isVirt;
    QList<RateVal> rates;
    QList<Unit> tagList;
    bool isAbbrevList;
};
