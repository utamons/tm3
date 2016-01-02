#pragma once
#include <functional>
#include "dbhelper.h"

template <typename T>

class InsertFunctor {
public:

    void operator()(T &entity) {
        if (funcPrepare != nullptr) {
            QSqlQuery q(getDb());
            funcPrepare(q,entity);
            execQuery(q);

            QVariant id = q.lastInsertId();
            if (id.isValid())
                entity.setId(id);
        }
    }

    void operator=(std::function<void (QSqlQuery&, const T&)> f) {
        funcPrepare = f;
    }

private:
    std::function<void(QSqlQuery&, const T&)> funcPrepare;
};


