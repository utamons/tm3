#include "ratevaltablemodel.h"
#include <QtGlobal>
#include "dbhelper.h"

RateValTableModel::RateValTableModel(QList<RateVal> rowList, QObject *parent) :
    BaseTableModel<RateVal>(parent) {
    this->rowList = rowList;
    colCount = 3;
}

QVariant RateValTableModel::headerData(int section, Qt::Orientation,
                                       int role) const {
    QVariant result;
    if (role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            result = QVariant(tr("Rate"));
            break;
        case 1:
            result = QVariant(tr("Value"));
            break;
        case 2:
            result = QVariant(tr("Unit"));
            break;
        }
    }
    return result;
}


void RateValTableModel::replaceList(const QList<RateVal> &newList) {
    beginResetModel();
    rowList = newList;
    endResetModel();
}
