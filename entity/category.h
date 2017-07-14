#pragma once

#include <QList>
#include <QString>
#include "rateval.h"
#include "entity.h"
#include <QDebug>

class Category : public Entity {
public:
    Category() {
        id = 0;
        name = "";
        isAbbrevList = false;
    }

    QString toString() const override {
        return isAbbrevList?abbrev:name;
    }

    bool isEmpty() const override {
        return id == 0 || name.isEmpty();
    }

    void setId (QVariant id) override { this->id = id.toInt(); }

    int id;
    QString name;
    QString comment;
    QString abbrev;
    bool isVirt;
    QList<RateVal> rates;
    QList<Unit> tagList;
    bool isAbbrevList;
};
