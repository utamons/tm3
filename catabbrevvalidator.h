#pragma once
#define CATABBREVVALIDATOR_H

#include <QValidator>
#include "category.h"

class CatAbbrevValidator : public QValidator
{
    Q_OBJECT
public:
    explicit CatAbbrevValidator(QList<Category> catList,QObject *parent = nullptr);
    
    QValidator::State validate(QString &str, int &) const override;
    void reload();


private:
    const QList<Category> catList;
    QString hasAbbrev(QString str) const;
};
