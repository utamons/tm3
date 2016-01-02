#pragma once

#include "unit.h"
#include "tm3util.h"

class Rate : public Entity {
public:
    Rate () {
        id = 0;
    }

    Rate (int id, QString name, QString comment, Unit unit, int time) {
        this->id = id;
        this->name = name;
        this->comment = comment;
        this->unit = unit;
        this->time = time;
    }

    friend bool operator <(const Rate& one, const Rate& two) {
        return one.id < two.id;
    }

    Unit unit;
    QString name;
    QString comment;
    int time,id;

    bool isEmpty() const override {
        return name.isEmpty() || unit.isEmpty();
    }

    QString toString() const override {
        QString summary = name.trimmed() + " (" + unit.name;

        if (time>0) {
            summary = summary + "/" + QString::number(time) + tr(" мин");
        }

        summary.append(")");
        return summary;
    }

    void setId(QVariant id) override { this->id = id.toInt(); }
};

