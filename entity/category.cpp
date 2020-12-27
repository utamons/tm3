#include "category.h"


Category::Category() {
    id = 0;
    name = "";
    isAbbrevList = false;
}

QString Category::toString() const {
    return isAbbrevList?abbrev:name;
}

bool Category::isEmpty() const {
    return id == 0 || name.isEmpty();
}

void Category::setId(QVariant id) { this->id = id.toInt(); }
