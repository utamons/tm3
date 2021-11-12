#pragma once

#include "rate.h"
#include "entity.h"

class RateVal : public Entity  {

public:
    RateVal ();

    bool operator==(const RateVal &that) const;

    bool isEmpty() const override;

    QList<QVariant> toTableRow() const override;

    Rate rate;
    double value;
};
