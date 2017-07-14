#pragma once

#include <functional>
#include "dbhelper.h"

template <typename T>

class DeleteFunctor {
public:

    DeleteFunctor() {
        isFuncSet = false;
    }

    SqlQueryStatus operator()(int idx) {
        SqlQueryStatus result= SQL_OK;

        if (isFuncSet) {
            QSqlQuery q(getDb());
            funcPrepare(q,idx);
            result = execQuery(q,true);
        }

        return result;
    }

    void operator=(std::function<void (QSqlQuery&, int)> f) {
        isFuncSet = true;
        funcPrepare = f;
    }

private:
    bool isFuncSet;
    std::function<void(QSqlQuery&, int)> funcPrepare;
};


