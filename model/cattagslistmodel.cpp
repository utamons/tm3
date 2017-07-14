#include "cattagslistmodel.h"
#include "dbhelper.h"

CatTagsListModel::CatTagsListModel(const QList<Unit> &rowList, QObject *parent)  :
    BaseListModel<Unit>(parent)  {
    this->rowList = rowList;
}


