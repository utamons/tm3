#include <QMessageBox>
#include <QApplication>
#include <QtAlgorithms>
#include <QtGlobal>
#include "activitytablemodel.h"
#include "tm3util.h"
#include <QDebug>

bool actCompare(const Activity &one, const Activity &two) {
    return one.time < two.time;
}

void ActivityTableModel::insert(Activity act) {
    Q_ASSERT(!act.isEmpty());

    QSqlQuery q(getDb());

    beginTransaction();
    q.prepare(
                "insert into actions (comment,cat_id,time,mins) values (:comment,:cat_id,:time,:mins)");
    q.bindValue(":comment", act.comment);
    q.bindValue(":cat_id", act.cat.id);
    q.bindValue(":time", toEpochMins(act.time));
    q.bindValue(":mins", act.mins);

    execQuery(q);

    act.id = q.lastInsertId().toInt();

    for(RateVal rv: act.rateValList) {
        q.prepare("insert into act_rate_link (act_id,rate_id,value) values (:act_id,:rate_id,:value)");
        q.bindValue(":act_id",act.id);
        q.bindValue(":rate_id",rv.rate.id);
        q.bindValue(":value",rv.value);
        execQuery(q);
    }

    for(Unit u: act.tagList) {
        q.prepare("insert into act_tag_link (act_id,tag_id) values (:act_id,:tag_id)");
        q.bindValue(":act_id",act.id);
        q.bindValue(":tag_id",u.id);
        execQuery(q);
    }

    commit();

    if (act.time.date() == currentDate) {
        beginInsertRows(QModelIndex(), rowList.length(), rowList.length());
        rowList.append(act);
        std::sort(rowList.begin(), rowList.end(), actCompare);
        endInsertRows();
    }
}

void ActivityTableModel::update(Activity act, const QModelIndex &row) {
    if (row.isValid()) {
        QSqlQuery q(getDb());
        beginTransaction();

        q.prepare("delete from act_rate_link where act_id=:id");
        q.bindValue(":id", act.id);

        execQuery(q);

        q.prepare("delete from act_tag_link where act_id=:id");
        q.bindValue(":id", act.id);

        execQuery(q);

        q.prepare("update actions set time=:time,"
                  "cat_id=:cat_id, mins=:mins, comment=:comment where id=:id");
        q.bindValue(":time", toEpochMins(act.time));
        q.bindValue(":cat_id", act.cat.id);
        q.bindValue(":mins", act.mins);
        q.bindValue(":comment", act.comment);
        q.bindValue(":id", act.id);

        execQuery(q);

        for(RateVal rv: act.rateValList){
            q.prepare("insert into act_rate_link (act_id,rate_id,value) values (:act_id,:rate_id,:value)");
            q.bindValue(":act_id",act.id);
            q.bindValue(":rate_id",rv.rate.id);
            q.bindValue(":value",rv.value);
            execQuery(q);
        }

        for(Unit u: act.tagList) {
            q.prepare("insert into act_tag_link (act_id,tag_id) values (:act_id,:tag_id)");
            q.bindValue(":act_id",act.id);
            q.bindValue(":tag_id",u.id);
            execQuery(q);
        }

        commit();

        if (act.time.date() != currentDate) {
            beginRemoveRows(QModelIndex(), row.row(), row.row());
            rowList.removeAt(row.row());
            endRemoveRows();
        } else {
            rowList.replace(row.row(),act);
            std::sort(rowList.begin(), rowList.end(), actCompare);
        }
    }
}

SqlQueryStatus ActivityTableModel::remove(const QModelIndex &row) {
    if (row.isValid()) {
        QSqlQuery q(getDb());
        beginTransaction();

        Activity act = entityFromIndex(row);

        q.prepare("delete from act_rate_link where act_id=:id");
        q.bindValue(":id", act.id);

        execQuery(q);

        q.prepare("delete from act_tag_link where act_id=:id");
        q.bindValue(":id",  act.id);

        execQuery(q);

        q.prepare("delete from actions where id=:id");
        q.bindValue(":id",  act.id);

        execQuery(q);

		// Is the action divided by days border
        QDateTime nextDt = act.time.addSecs(act.mins * 60);
        if (nextDt.time().hour() == 0 && nextDt.time().minute() == 0) {
            QTime time(0, 0, 0, 0);
			nextDt.setTime(time); // set seconds to zero
            q.prepare("select id,cat_id from actions where time=:time");
            q.bindValue(":time", toEpochMins(nextDt));
            execQuery(q, [act,q]() {
                if (act.cat.id == field<int>(q,"cat_id")) {
                    QSqlQuery qDel(getDb());
                    int id = field<int>(q,"id");
                    qDel.prepare("delete from act_rate_link where act_id=:id");
                    qDel.bindValue(":id", id);

                    execQuery(qDel);

                    qDel.prepare("delete from act_tag_link where act_id=:id");
                    qDel.bindValue(":id", id);

                    execQuery(qDel);

                    qDel.prepare("delete from actions where id=:id");
                    qDel.bindValue(":id", id);

                    execQuery(qDel);
                }
            });
        }

        commit();

        beginRemoveRows(QModelIndex(), row.row(), row.row());
        rowList.removeAt(row.row());
        endRemoveRows();
    }
    return SQL_OK;
}

QVariant ActivityTableModel::headerData(int section, Qt::Orientation,
                                        int role) const {
    QVariant result;
    if (role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            result = QVariant(tr("Date and time"));
            break;
        case 1:
            result = QVariant(tr("Duration"));
            break;
        case 2:
            result = QVariant(tr("Category"));
            break;
        case 3:
            result = QVariant(tr("Comment"));
            break;
        }
    }
    return result;
}

bool ActivityTableModel::isCrossTime(QDateTime beginTime, int mins, int selfId) {
    bool result = false;
    QSqlQuery q(getDb());


    if (beginTime.time().hour() == 0 && beginTime.time().minute() == 0) {
        beginTime = beginTime.addSecs(60);
        --mins;
    }

    q.prepare(
                "select count(id) as cnt from actions where id<>:self and ((:begin<=time and :end>time) "
                "or (:begin1>time and (time+mins)>:begin2))");
    q.bindValue(":self", selfId);
    q.bindValue(":begin", toEpochMins(beginTime));
    q.bindValue(":begin1", toEpochMins(beginTime));
    q.bindValue(":begin2", toEpochMins(beginTime));
    q.bindValue(":end",	toEpochMins(beginTime.addSecs(mins * 60)));

    execQuery(q, [&q,&result]() {
        result = (field<int>(q,"cnt") > 0);
    });

    return result;
}

// Time after last action.
QDateTime ActivityTableModel::getLastTime() {
    QSqlQuery q(getDb());
    q.prepare("select max(time) as m from actions");

    QDateTime maxTime, result;
    int mins = 0;
    bool found = false;
    execQuery(q, [q,&maxTime,&found]() {
        maxTime = toDateTime(field<long>(q,"m"));
        found = maxTime.toMSecsSinceEpoch()>0;
    });

    if (found) {
        q.prepare("select mins from actions where time=:time");
        q.bindValue(":time", toEpochMins(maxTime));
        execQuery(q, [q,&mins]() {mins = field<int>(q,"mins");});
        Q_ASSERT(mins > 0);
        result = maxTime.addSecs(mins * 60);
    } else {
        result = QDateTime::currentDateTime();
    }

    return result;
}

void ActivityTableModel::init(QDateTime begin, QDateTime end) {
    currentDate = begin.date();
    QSqlQuery q(getDb());
    rowList.clear();
    q.prepare(
                "select a.id as aid, a.time, a.mins, a.comment, a.cat_id,"
                " c.name as cname, c.abbrev from actions a, cat_rb c where a.cat_id=c.id"
                " and a.time>=:begin and a.time<:end");
    q.bindValue(":begin", toEpochMins(begin));
    q.bindValue(":end", toEpochMins(end));

    beginResetModel();

    execQuery(q,
              [q,this]() {
        Activity act;
        act.id = field<int>(q,"aid");
        act.cat.id = field<int>(q,"cat_id");
        act.time = toDateTime(field<long>(q,"time"));
        act.mins = field<int>(q,"mins");
        act.cat.name = field<QString>(q,"cname");
        act.cat.abbrev = field<QString>(q,"abbrev");
        act.comment = field<QString>(q,"comment");

        fillData(act);

        rowList.append(act);
    });

    std::sort(rowList.begin(), rowList.end(), actCompare);

    endResetModel();
}

void ActivityTableModel::fillData(Activity &act) {
    QSqlQuery q(getDb());

    q.prepare(
                "select l.value as value, r.id as rid, r.name as rname, u.name as unitname, "
                "r.time as rtime, r.comment as rcomment, u.id as uid, u.name as uname "
                "from act_rate_link l, rate_rb r, unit_rb u where "
                "l.rate_id=r.id and r.unit_id=u.id and l.act_id=:act_id");
    q.bindValue(":act_id", act.id);

    execQuery(q, [q,this,&act] () {
        RateVal rateVal;
        rateVal.value = field<double>(q,"value");
        rateVal.rate.id = field<int>(q,"rid");
        rateVal.rate.name = field<QString>(q,"rname");
        rateVal.rate.time = field<int>(q,"rtime");
        rateVal.rate.comment = field<QString>(q,"rcomment");
        rateVal.rate.unit = Unit(field<int>(q,"uid"), field<QString>(q,"uname"));

        act.rateValList.append(rateVal);
    });

    q.prepare(
                "select l.tag_id as tid, t.name as tname "
                "from act_tag_link l, tag_rb t where "
                "l.tag_id=t.id and l.act_id=:act_id");
    q.bindValue(":act_id", act.id);

    execQuery(q, [q,this,&act] () {
        act.tagList.append(Unit(field<int>(q,"tid"),
                                field<QString>(q,"tname")));
    });

}
