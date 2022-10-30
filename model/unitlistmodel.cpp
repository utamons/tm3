#include "unitlistmodel.h"
#include <QDebug>

UnitListModel::UnitListModel(QObject *parent) : BaseListModel<Unit>(parent) {

    QSqlQuery q(getDb());

    q.prepare("select id , name from unit_rb");

    execQuery(q, [&q,this]() {
        rowList.append(
                    Unit(field<int>(q,"id"), field<QString>(q,"name"))
                    );
    });

    insertFunc = [] (QSqlQuery &q, const Unit &unit) {
        q.prepare(
                    "insert into unit_rb (name) values (:name)");
        q.bindValue(":name", unit.name);
    };

    updateFunc = [] (QSqlQuery &q, const Unit &unit) {
        q.prepare(
                    "update unit_rb set name=:name where id=:id");
        q.bindValue(":name", unit.name);
        q.bindValue(":id", unit.id);
    };

    deleteFunc = [this] (QSqlQuery &q, int idx) {
        q.prepare("delete from unit_rb where id=:id");
        q.bindValue(":id",  rowList[idx].id);
    };
}

void UnitListModel::prepareDBMaxRecords(QSqlQuery &q) {
    q.prepare("select count(id) from unit_rb");
}

void UnitListModel::prepareDBUnique(QSqlQuery &q, Unit unit) {
    q.prepare("select count(id) from unit_rb where id <> :id and name=:name");
    q.bindValue(":id",unit.id);
    q.bindValue(":name",unit.name);
}



