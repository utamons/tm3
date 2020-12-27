#include "activity.h"

Activity::Activity() {
    id = 0;
    mins = 0;
}

bool Activity::isEmpty() const {
    return cat.isEmpty() || mins == 0;
}

QList<QVariant> Activity::toTableRow() const {
    QList<QVariant> row;
    row.append(time);
    row.append(timeFromMins(mins));
    row.append(cat.name);
    row.append(comment);
    return row;
}

void Activity::setId(QVariant id) {
    this->id = id.toInt();
}
