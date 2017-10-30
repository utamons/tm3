#include "tagcombomodel.h"

TagComboModel::TagComboModel(QList<Unit> excludes, QObject *parent, bool insertEmptyVal) : BaseListModel<Unit>(parent) {
    init();
    for (Unit u: excludes) {
        for (Unit t: rowList) {
            if (u.id == t.id) {
                rowList.removeOne(t);
                break;
            }
        }
    }
    if (insertEmptyVal)
        rowList.insert(0,Unit());
}

TagComboModel::TagComboModel(bool insertEmptyVal, QObject *parent) : BaseListModel<Unit>(parent) {
    init();
    if (insertEmptyVal)
        rowList.insert(0,Unit());
}

int TagComboModel::idByName(QString name) {
	int result = 0;
    for(Unit u: rowList) {
        if (u.name == name) {
            result = u.id;
			break;
		}
	}
	return result;
}

Unit TagComboModel::tagById(int id) const {
    Unit result;
    for (Unit u: rowList) {
        if (u.id == id) {
			result = u;
			break;
		}
	}
    return result;
}

void TagComboModel::init() {
    QSqlQuery q(getDb());

    q.prepare("select id , name from tag_rb");

    execQuery(q, [q,this]() {
        rowList.append(
                    Unit(field<int>(q,"id"), field<QString>(q,"name"))
                    );
    });
}
