#pragma once

#include "entity.h"

class Unit  : public Entity {

public:
    Unit() {
        id = 0;
    }

    Unit (int id, QString name) {
        this->id = id;
        this->name = name;
    }

    bool operator == ( Unit const &that ) {
        return id = that.id && name == that.name;
    }

    QString name;
    int id;

    QString toString() const override {
        return name;
    }

    bool isEmpty() const override {
        return id == 0 && name.isEmpty();
    }

    void setId(QVariant id) override {
        this->id = id.toInt();
    }
};
