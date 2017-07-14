#include "acttagslistmodel.h"

ActTagsListModel::ActTagsListModel(const QList<Unit> &rowList, QObject *parent) :
    BaseListModel<Unit>(parent) {
    this->rowList = rowList;
}

void ActTagsListModel::replaceList(const QList<Unit> &rowList) {
    beginResetModel();
    this->rowList = rowList;
    endResetModel();
}

