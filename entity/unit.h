#pragma once

#include "entity.h"
#include <QDebug>

class Unit  : public Entity {

public:
    Unit();

    Unit (int id, QString name);

    Unit (const Unit &that);

    bool operator == ( Unit const &that ) const;

    Unit& operator = (const Unit& other);

    QString name;
	int id;

    QString toString() const override;

    bool isEmpty() const override;

    void setId(QVariant id) override;
};
