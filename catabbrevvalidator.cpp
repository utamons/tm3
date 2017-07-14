#include "catabbrevvalidator.h"
#include "dbhelper.h"

CatAbbrevValidator::CatAbbrevValidator(QList<Category> catList, QObject *parent) :
    QValidator(parent),catList(catList) {
}

QValidator::State CatAbbrevValidator::validate(QString &str, int &) const {
    if (hasAbbrev(str).length() == str.length())
        return Acceptable;
    else if (hasAbbrev(str).length() > str.length())
        return Intermediate;
    else
        return Invalid;
}

QString CatAbbrevValidator::hasAbbrev(QString str) const {
    QString result = "";
    for(auto cat:catList) {
        if (cat.abbrev.toUpper().indexOf(str.toUpper()) == 0) {
            result = cat.abbrev;
            break;
        }
    }
    return result;
}


