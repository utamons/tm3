#include "taglistmodel.h"

TagListModel::TagListModel(QObject *parent) : BaseListModel<Unit>(parent) {

    QSqlQuery q(getDb());

    q.prepare("select id , name from tag_rb");

    execQuery(q, [q,this]() {
        rowList.append(
                    Unit(field<int>(q,"id"), field<QString>(q,"name"))
                    );
    });

    insertFunc = [] (QSqlQuery &q, const Unit &tag) {
        q.prepare(
                    "insert into tag_rb (name) values (:name)");
        q.bindValue(":name", tag.name);
    };

    updateFunc = [] (QSqlQuery &q, const Unit &tag) {
        q.prepare(
                    "update tag_rb set name=:name where id=:id");
        q.bindValue(":name", tag.name);
        q.bindValue(":id", tag.id);
    };

    deleteFunc = [this] (QSqlQuery &q, int idx) {
        q.prepare("delete from tag_rb where id=:id");
        q.bindValue(":id",  rowList[idx].id);
    };
}

void TagListModel::prepareDBMaxRecords(QSqlQuery &q) {
    q.prepare("select count(id) from tag_rb");
}

void TagListModel::prepareDBUnique(QSqlQuery &q, Unit tag) {
    q.prepare("select count(id) from tag_rb where id <> :id and name=:name");
    q.bindValue(":id",tag.id);
    q.bindValue(":name",tag.name);
}

