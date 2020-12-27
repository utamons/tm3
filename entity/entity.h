#pragma once
#include <functional>
#include <QVariant>
#include <QList>

class Entity {
public:
    virtual void setId(QVariant) {}
    virtual QVariant getId() const {return QVariant();}
    virtual QString toString () const {return "";}
    virtual QList<QVariant> toTableRow() const {return QList<QVariant>();}
    virtual bool isEmpty() const {return false;}
    virtual ~Entity();
};
