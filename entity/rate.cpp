#include "rate.h"

Rate::Rate() {
    id = 0;
}

Rate::Rate(int id, QString name, QString comment, Unit unit, int time) {
    this->id = id;
    this->name = name;
    this->comment = comment;
    this->unit = unit;
    this->time = time;
}

bool Rate::isEmpty() const {
    return name.isEmpty() || unit.isEmpty();
}

QString Rate::toString() const {
    QString summary = name.trimmed() + " (" + unit.name;

    if (time>0) {
        summary = summary + "/" + QString::number(time) + tr(" мин");
    }

    summary.append(")");
    return summary;
}

void Rate::setId(QVariant id) { this->id = id.toInt(); }
