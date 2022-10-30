#include "unit.h"

Unit::Unit() {
    id = 0;
}

Unit::Unit(int id, QString name) {
    this->id = id;
    this->name = name;
}

Unit::Unit(const Unit &that) {
    this->id = that.id;
    this->name = that.name;
}

bool Unit::operator ==(const Unit &that) const{
    return id == that.id && name == that.name;
}

Unit &Unit::operator =(const Unit &other){
    if (this != &other) {
        this->id = other.id;
        this->name = other.name;
    }
    return *this;
}

QString Unit::toString() const {
    return name;
}

bool Unit::isEmpty() const {
    return id == 0 && name.isEmpty();
}

void Unit::setId(QVariant id) {
    this->id = id.toInt();
}

