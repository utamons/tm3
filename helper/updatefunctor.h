#pragma once

#include <functional>
#include "dbhelper.h"

template <typename T>

class UpdateFunctor {
public:

    UpdateFunctor() {
        isFuncSet = false;
    }

    void operator()(const T &entity) {
        if (isFuncSet) {
            QSqlQuery q(getDb());
            funcPrepare(q,entity);
            execQuery(q);
        }
    }

    void operator=(std::function<void (QSqlQuery&, const T&)> f) {
        isFuncSet = true;
        funcPrepare = f;
    }

private:
    bool isFuncSet;
    std::function<void(QSqlQuery&, const T&)> funcPrepare;
};

