#pragma once

#include <QList>
#include <QDateTime>
#include "category.h"
#include "entity.h"
#include <QDebug>
#include "tm3util.h"

class Activity : public Entity {
public:
    Activity() {
        id = 0;
        mins = 0;
    }

    bool isEmpty() const override {
        return cat.isEmpty() || mins == 0;
    }

    QList<QVariant> toTableRow() const override {
        QList<QVariant> row;
        row.append(time);
        row.append(timeFromMins(mins));
        row.append(cat.name);
        row.append(comment);
        return row;
    }

    void setId(QVariant id) override {
        this->id = id.toInt();
    }

    QList<RateVal> rateValList;
    QList<Unit> tagList;
    QDateTime time;
    Category cat;
    int mins;
    int id;
    QString comment;
};
