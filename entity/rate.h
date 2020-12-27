#pragma once

#include "unit.h"
#include "tm3util.h"

class Rate : public Entity {
public:
    Rate ();

    Rate (int id, QString name, QString comment, Unit unit, int time);

    friend bool operator <(const Rate& one, const Rate& two) {
        return one.id < two.id;
    }

    Unit unit;
    QString name;
    QString comment;
    int time,id;

    bool isEmpty() const override;

    QString toString() const override;

    void setId(QVariant id) override;
};

