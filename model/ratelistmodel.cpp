#include "ratelistmodel.h"
#include "dbhelper.h"

RateListModel::RateListModel(QList<Rate> excludes, QObject *parent) :
    BaseListModel<Rate>(parent) {

    QSqlQuery q(getDb());

    q.prepare("select r.id as rid, r.name as rname, r.comment as rcomment, "
              "r.time as rtime, u.id as uid, u.name as uname from "
              "rate_rb r, unit_rb u where r.unit_id=u.id order by r.name");

    execQuery(q, [&q,excludes,this]() {
        int rid = field<int>(q,"rid");
        for (Rate rate: excludes) {
            if (rate.id == rid)
                return;
        }
        rowList.append(
                    Rate (rid,
                          field<QString>(q,"rname"),
                          field<QString>(q,"rcomment"),
                          Unit(field<int>(q,"uid"), field<QString>(q,"uname")),
                          field<int>(q,"rtime"))
                    );
    });

    insertFunc = [] (QSqlQuery &q, const Rate &rate) {
        q.prepare(
                    "insert into rate_rb (name,comment,time,unit_id) values (:name,:comment,:time,:unit_id)");
        q.bindValue(":name", rate.name);
        q.bindValue(":comment", rate.comment);
        q.bindValue(":time", rate.time);
        q.bindValue(":unit_id", rate.unit.id);
    };

    updateFunc = [] (QSqlQuery &q, const Rate &rate) {
        q.prepare(
                    "update rate_rb set name=:name, comment=:comment, time=:time, unit_id=:unit_id where id=:id");
        q.bindValue(":name", rate.name);
        q.bindValue(":comment", rate.comment);
        q.bindValue(":time", rate.time);
        q.bindValue(":unit_id", rate.unit.id);
        q.bindValue(":id", rate.id);
    };

    deleteFunc = [this] (QSqlQuery &q, int idx) {
        q.prepare("delete from rate_rb where id=:id");
        q.bindValue(":id",  rowList[idx].id);
    };
}

int RateListModel::idBySummary(QString name) const {
    int result = -1;
    for (Rate r: rowList) {
        if ( r.toString() == name) {
            result =  r.id;
            break;
        }
    }
    return result;
}

Rate RateListModel::rateById(int id) const {
    Rate result;
    for (Rate r: rowList) {
        if (r.id == id) {
            result = r;
            break;
        }
    }
    return result;
}

void RateListModel::prepareDBMaxRecords(QSqlQuery &q) {
    q.prepare("select count(id) from rate_rb");
}

void RateListModel::prepareDBUnique(QSqlQuery &q, Rate rate) {
    q.prepare("select count(id) from rate_rb where id <> :id and name=:name");
    q.bindValue(":id",rate.id);
    q.bindValue(":name",rate.name);
}
