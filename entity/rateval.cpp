#include "rateval.h"
#include "tm3util.h"

RateVal::RateVal() {
    value = 0.0;
}

bool RateVal::operator==(const RateVal &that) const {
    return !that.isEmpty() && that.rate.id == this->rate.id && that.value == this->value;
}

bool RateVal::isEmpty() const {
    return rate.isEmpty();
}

QList<QVariant> RateVal::toTableRow() const {
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
