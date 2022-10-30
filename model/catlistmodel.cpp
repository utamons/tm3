#include "catlistmodel.h"
#include "dbhelper.h"
#include "cattreemodel.h"

CatListModel::CatListModel(QObject *parent) :  BaseListModel<Category>(parent) {
    QSqlQuery q(getDb());
    q.prepare("select id,name,abbrev from cat_rb where virtual=0 order by abbrev");
    execQuery(q,[&q,this]() {
            Category c;
            c.id = field<int>(q,"id");
            c.abbrev = field<QString>(q,"abbrev");
            c.name = field<QString>(q,"name");
            c.isAbbrevList = true;

            CatTreeModel::fillData(c);

            rowList.append(c);
    });
}

Category CatListModel::byAbbrev(QString abbrev) {
    Category result;
    for(auto c: rowList) {
        if (c.abbrev.toUpper() == abbrev.toUpper()) {
            result = c;
            break;
        }
    }
	return result;
}

Category CatListModel::byId(long id) {
    Category result;
    for(auto c: rowList) {
        if (c.id == id) {
			result = c;
			break;
		}
	}
    return result;
}

void CatListModel::prepareDBUnique(QSqlQuery &q, Category cat) {
    q.prepare("select count(id) from cat_rb where (abbrev=:abbrev or name=:name) and id <> :id");
    q.bindValue(":abbrev",cat.abbrev);
    q.bindValue(":name",cat.name);
    q.bindValue(":id",cat.id);
}
