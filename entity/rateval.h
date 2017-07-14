#pragma once

#include "rate.h"
#include "entity.h"

class RateVal : public Entity  {

public:
    RateVal () {
        value = 0.0;
    }

    bool operator==(const RateVal &that) {
        return !that.isEmpty() && that.rate.id == this->rate.id && that.value == this->value;
    }

    bool isEmpty() const override {
        return rate.isEmpty();
    }

    QList<QVariant> toTableRow() const override {
        QList<QVariant> row;
        row.append(rate.name);
        row.append(value);
        QString uData = rate.unit.name;
        if (rate.time > 0) {
            uData.append("/" + QString::number(rate.time) + tr(" мин"));
        }
        row.append(QVariant(uData));
        return row;
    }

    Rate rate;
    double value;
};
