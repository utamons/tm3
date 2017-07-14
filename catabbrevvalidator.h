#pragma once
#define CATABBREVVALIDATOR_H

#include <QValidator>
#include "Category.h"

class CatAbbrevValidator : public QValidator
{
    Q_OBJECT
public:
    explicit CatAbbrevValidator(QList<Category> catList,QObject *parent = 0);
    
    QValidator::State validate(QString &str, int &) const;
    void reload();


private:
    const QList<Category> catList;
    QString hasAbbrev(QString str) const;
};
