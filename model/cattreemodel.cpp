#include "cattreemodel.h"
#include "tm3util.h"
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>
#include <qdebug.h>

CatTreeModel::CatTreeModel(QObject *parent) :
    QAbstractItemModel(parent) {
    root = new TreeNode<Category>();
    buildBranch(root);
}

QVariant CatTreeModel::data(const QModelIndex &index, int role) const {
    QVariant result;
    auto cat = nodeFromIndex(index)->data();

    if (role == Qt::DisplayRole && index.column() == 0) {
        result = QVariant(cat.name.trimmed());
    }

    return result;
}

int CatTreeModel::rowCount(const QModelIndex &parent) const {
    return nodeFromIndex(parent)->childrenCount();
}

QMimeData *CatTreeModel::mimeData(const QModelIndexList &indexes) const {
    auto mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    auto index = indexes.first();
    if (index.isValid()) {
        stream <<  nodeFromIndex(index)->getId();
    }

    mimeData->setData("text/plain", encodedData);
    return mimeData;
}

QStringList CatTreeModel::mimeTypes() const {
    QStringList types;
    types << "text/plain";
    return types;
}


bool CatTreeModel::moveCat(TreeNode<Category> *movedCat, const QModelIndex& parentIdx) {
    int childCount = rowCount(parentIdx);
    auto movedTo = nodeFromIndex(parentIdx);
    QModelIndex movedIdx;
    if (!movedCat->getParent()->isRoot())
        movedIdx = createIndex(movedCat->getParent()->row(), 0, movedCat->getParent());

    beginRemoveRows(movedIdx, movedCat->row(), movedCat->row());
    movedCat->getParent()->removeChild(movedCat);
    endRemoveRows();

    beginInsertRows(parentIdx, childCount, childCount);
    movedTo->addChild(movedCat);
    QSqlQuery q(getDb());
    q.prepare("update cat_rb set parent_id=:parent_id where id=:id");
    q.bindValue(":parent_id", movedTo->getId());
    q.bindValue(":id", movedCat->getId());
    execQuery(q);
    endInsertRows();
    emit dropped(getIndex(movedCat));
    return true;
}

bool CatTreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int, int, const QModelIndex &parentIdx) {
    bool result = false;
    if (data->hasFormat("text/plain")) {
        auto encodedData = data->data("text/plain");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        int movedId;
        stream >> movedId;
        auto movedCat = root->findById(movedId);
        Q_ASSERT(movedCat != nullptr);
        auto movedTo = nodeFromIndex(parentIdx);

        if (movedTo->isRoot()) {
            result =  moveCat(movedCat, parentIdx);
        } else {
            QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Move or join?"));
			msgBox.setText(tr("Do you want to move or join categories?"));
			auto moveButton = msgBox.addButton(tr("Move"), QMessageBox::ActionRole);
			auto joinButton = msgBox.addButton(tr("Join"), QMessageBox::ActionRole);
			msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);

            msgBox.exec();

            if (msgBox.clickedButton() == joinButton && scaryQuestion(movedCat->data(), movedTo->data())) {
                joinCat(movedCat, parentIdx);
            } else if (msgBox.clickedButton() == moveButton && action != Qt::IgnoreAction) {
                result = moveCat(movedCat, parentIdx);
            }
        }

    }
    return result;
}



Qt::ItemFlags CatTreeModel::flags(const QModelIndex &index) const {
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        return Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

TreeNode<Category> *CatTreeModel::nodeFromIndex(const QModelIndex& index) const {
    if (index.isValid())
        return static_cast<TreeNode<Category>*>(index.internalPointer());
    else
        return root;
}



int CatTreeModel::addNode(Category cat, const QModelIndex &index) {
    auto parent = nodeFromIndex(index);
    int childCount = parent->childrenCount();
    int newid = 0;

    QSqlQuery q(getDb());

    beginTransaction();

    q.prepare("insert into cat_rb (name,abbrev,comment,virtual, expanded, parent_id) values "
              "( :name, :abbrev, :comment, :virtual, 0, :parent_id )");

    q.bindValue(":name", cat.name);
    q.bindValue(":abbrev", cat.abbrev);
    q.bindValue(":comment", cat.comment);
    q.bindValue(":virtual", cat.isVirt ? 1 : 0);
    q.bindValue(":parent_id", parent->getId());

    execQuery(q);

    newid = q.lastInsertId().toInt();

    for (auto rate: cat.rates){
        QSqlQuery q(getDb());
        q.prepare("insert into cat_rate_link (default_value,cat_id,rate_id) values "
                  "( :value,:catid,:rateid )");
        q.bindValue(":value",rate.value);
        q.bindValue(":catid",newid);
        q.bindValue(":rateid",rate.rate.id);

        execQuery(q);
    }

    for(auto u: cat.tagList) {
        q.prepare("insert into cat_tag_link (cat_id,tag_id) values (:cat_id,:tag_id)");
        q.bindValue(":cat_id",newid);
        q.bindValue(":tag_id",u.id);
        execQuery(q);
    }

    beginInsertRows(index, childCount, childCount);
    cat.id = newid;
    parent->addChild(new TreeNode<Category>(newid,cat));
    endInsertRows();

    commit();

    return newid;
}

QModelIndex CatTreeModel::deleteNode(const QModelIndex &index) {
    auto node = nodeFromIndex(index);
    QModelIndex result;

    if (node->hasChildren()) {
			QMessageBox::critical(nullptr, tr("Error"), tr("Cannot delete this branch!"));
    } else {

        int row = node->row();

        QSqlQuery q(getDb());

        beginTransaction();

        q.prepare("delete from cat_rate_link where cat_id=:catid");
        q.bindValue(":catid", node->getId());
        execQuery(q);

        q.prepare("delete from cat_rb where id=:catid");
        q.bindValue(":catid", node->getId());
        SqlQueryStatus status = execQuery(q,true);

        if (status == SQL_OK) {

            TreeNode<Category> *node2Select;

            if (node->getParent()->childrenCount() == 1) {
                node2Select = node->getParent();
            } else {
                node2Select = node->getParent()->childAt(row-1<1?0:row-1);
            }

            beginRemoveRows(index, row, row);
            node->getParent()->removeChild(node);
            endRemoveRows();

            commit();

            zombies.append(node);

            result = getIndex(node2Select);

        } else {
            rollback();
            if (status == FOREIGN_KEY_FAIL)
				QMessageBox::critical(nullptr, tr("Error"), tr("Category is in use!"));
        }
    }
    return result;
}

void CatTreeModel::updateNode(Category cat, const QModelIndex &index) {
    auto node = nodeFromIndex(index);
    Q_ASSERT (node->getId() == cat.id);

    QSqlQuery q(getDb());

    beginTransaction();

    q.prepare("delete from cat_rate_link where cat_id=:catid");
    q.bindValue(":catid", node->getId());
    execQuery(q);

    q.prepare("delete from cat_tag_link where cat_id=:id");
    q.bindValue(":id", cat.id);

    execQuery(q);

    for (auto rate: cat.rates){
        QSqlQuery q(getDb());
        q.prepare("insert into cat_rate_link (default_value,cat_id,rate_id) values "
                  "( :value,:catid,:rateid )");
        q.bindValue(":value",rate.value);
        q.bindValue(":catid",node->getId());
        q.bindValue(":rateid",rate.rate.id);

        execQuery(q);
    }

    for(auto u: cat.tagList) {
		QSqlQuery q(getDb());
        q.prepare("insert into cat_tag_link (cat_id,tag_id) values (:cat_id,:tag_id)");
        q.bindValue(":cat_id",cat.id);
        q.bindValue(":tag_id",u.id);
        execQuery(q);
    }

    q.prepare("update cat_rb set "
              "name=:name, "
              "comment=:comment,"
              "abbrev=:abbrev,"
              "virtual=:virtual "
              "where id=:catid");

    q.bindValue(":catid", node->getId());
    q.bindValue(":name", cat.name);
    q.bindValue(":abbrev", cat.abbrev);
    q.bindValue(":comment", cat.comment);
    q.bindValue(":virtual", cat.isVirt ? 1 : 0);
    execQuery(q);

    commit();

    node->setData(cat);
}

void CatTreeModel::expandNode(const QModelIndex &index, bool expand) {
    auto node = nodeFromIndex(index);
    node->setExpanded(expand);
    QSqlQuery q(getDb());
    q.prepare("update cat_rb set "
              "expanded=:expand "
              "where id=:catid");

    q.bindValue(":catid", node->getId());
    q.bindValue(":expand", expand ? 1 : 0);
    execQuery(q);
}

Category CatTreeModel::categoryFromIndex(const QModelIndex &index) {
    return  nodeFromIndex(index)->data();
}

QModelIndex CatTreeModel::getIndex(Category cat) {
    auto node = root->findById(cat.id);
    if (node == nullptr)
        return QModelIndex();
    else
        return getIndex(node);
}

QModelIndex CatTreeModel::index(int row, int column, const QModelIndex &parent) const {
    auto result = QModelIndex();
    auto parentNode = nodeFromIndex(parent);
    if (parentNode->childrenCount() > row && row >= 0) {
        result = createIndex(row, column, parentNode->childAt(row));
    }
    return result;
}

QModelIndex CatTreeModel::getIndex(TreeNode<Category> *node) {
    return createIndex(node->row(), 0, node);
}

QModelIndex CatTreeModel::parent(const QModelIndex &index) const {
    auto node = nodeFromIndex(index);
    if (node != nullptr && node->getParent() != nullptr && node->getParent() != root) {
        return createIndex(node->getParent()->row(), 0, node->getParent());
    } else
        return QModelIndex();
}

void CatTreeModel::buildBranch(TreeNode<Category> *parent) {
    QSqlQuery q(getDb());
    q.prepare("select id,name,abbrev,virtual,comment,expanded "
              "from cat_rb where parent_id=:parentId");
    q.bindValue(":parentId", parent->getId());
    execQuery(q, [&q,this,parent] () {
        Category cat;
        cat.id = field<int>(q,"id");
        cat.name = field<QString>(q,"name");
        cat.abbrev = field<QString>(q,"abbrev");
        cat.comment = field<QString>(q,"comment");
        cat.isVirt = field<bool>(q,"virtual");
        fillData(cat);

        auto newNode = new TreeNode<Category>(cat.id,cat);
        newNode->setExpanded(field<bool>(q,"expanded"));

        buildBranch(newNode);
        parent->addChild(newNode);
    });
}

void CatTreeModel::fillData(Category &cat) {
    QSqlQuery q(getDb());

    q.prepare("select l.default_value as def_value, r.id as rid, r.name as rname, u.name as unitname, "
              "r.time as rtime, r.comment as rcomment, u.id as uid, u.name as uname "
              "from cat_rate_link l, rate_rb r, unit_rb u where "
              "l.rate_id=r.id and r.unit_id=u.id and l.cat_id=:cat_id");
    q.bindValue(":cat_id", cat.id);
    execQuery(q, [&q,&cat] () {
        RateVal rateVal;
        rateVal.value = field<double>(std::move(q),"def_value");
        rateVal.rate.id = field<int>(std::move(q),"rid");
        rateVal.rate.name = field<QString>(std::move(q),"rname");
        rateVal.rate.time = field<int>(std::move(q),"rtime");
        rateVal.rate.comment = field<QString>(std::move(q),"rcomment");
        rateVal.rate.unit = Unit (field<int>(std::move(q),"uid"), field<QString>(std::move(q),"uname"));

        cat.rates.append(rateVal);
    });

    q.prepare(
                "select l.tag_id as tid, t.name as tname "
                "from cat_tag_link l, tag_rb t where "
                "l.tag_id=t.id and l.cat_id=:cat_id");
    q.bindValue(":cat_id", cat.id);

    execQuery(q, [&q, &cat] () {
        cat.tagList.append(Unit(field<int>(q,"tid"),
                                field<QString>(q,"tname")));
    });
}

bool CatTreeModel::scaryQuestion(Category movedCat, Category movedTo) {
    QMessageBox msgBox;

	msgBox.addButton(tr("I'm scared, get out from here!"), QMessageBox::ActionRole);
	QPushButton *yes = msgBox.addButton(tr("Nevermind, join!"), QMessageBox::ActionRole);

    QString redB = "<span style=\" color:#ff0000;\">";
    QString redE = "</span>";
    QString movedName = redB + movedCat.name + redE;
    QString movedToName = redB + movedTo.name + redE;

    QString msg = "<div style=\"width:400px \">" +
	        tr("<b><center>ATTENTION!</center><p>") +
	        tr("You're going to join ") + movedToName +
	        tr(" and ") +  movedName + "</p><p>" +
	        tr("All actions from ") + movedName + tr(" will move to ") +
            movedToName + "!</p><p>" +
	        movedName + tr(" will be removed!</p><p>") +
	        tr("You cannot roll back these changes! Are you scared?</p></b><br/>") +  "</div>";

    msgBox.setWindowTitle(" ");
    msgBox.setText(msg);
    msgBox.exec();

    return msgBox.clickedButton() == yes;
}

void CatTreeModel::joinCat(TreeNode<Category> *movedCat, const QModelIndex& parentIdx) {
    auto movedTo = nodeFromIndex(parentIdx);
    QSqlQuery q(getDb());
    beginTransaction();

	// Change deleted category to new one
    q.prepare("update actions set cat_id=:parent_id where cat_id=:id");
    q.bindValue(":parent_id", movedTo->getId());
    q.bindValue(":id", movedCat->getId());
    execQuery(q);

	// Move childs to new parent
    q.prepare("update cat_rb set parent_id=:parent_id where parent_id=:id");
    q.bindValue(":parent_id", movedTo->getId());
    q.bindValue(":id", movedCat->getId());
    execQuery(q);

	// Remove all default rates for deleted category
    q.prepare("delete from cat_rate_link where cat_id=:catid");
    q.bindValue(":catid", movedCat->getId());
    execQuery(q);

	// Remove category
    q.prepare("delete from cat_rb where id=:catid");
    q.bindValue(":catid", movedCat->getId());
    execQuery(q);

    commit();

    QModelIndex movedIdx;
    if (!movedCat->getParent()->isRoot())
        movedIdx = createIndex(movedCat->getParent()->row(), 0, movedCat->getParent());

    beginRemoveRows(movedIdx, movedCat->row(), movedCat->row());
    movedCat->getParent()->removeChild(movedCat);
    endRemoveRows();

    if (movedCat->hasChildren()) {
        int first = movedTo->childrenCount();
        int last = first + movedCat->childrenCount() - 1;

        beginInsertRows(parentIdx,first,last);
        TreeNode<Category>::moveChildren(movedTo,movedCat);
        endInsertRows();
    }

    zombies.append(movedCat);

    emit dropped(parentIdx);
}
