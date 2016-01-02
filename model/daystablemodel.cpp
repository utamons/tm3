#include "daystablemodel.h"
#include "dayshelper.h"
#include <QDebug>
#include <QColor>
#include <QDate>
#include <QFont>

DaysTableModel::DaysTableModel(QObject *parent) :
		QAbstractTableModel(parent) {
}

int DaysTableModel::rowCount(const QModelIndex&) const {
	return 6;
}

int DaysTableModel::columnCount(const QModelIndex&) const {
	return 7;
}

QVariant DaysTableModel::data(const QModelIndex& index, int role) const {
	QVariant result;
	if (index.isValid() && role == Qt::DisplayRole) {
        if (index.row() < 6 && index.column() < 7 && !days[index.row()][index.column()].isEmpty() ) {
            result = QVariant(days[index.row()][index.column()].mday);
		} else
			result = QVariant("");
	} else if (role == Qt::TextAlignmentRole) {
		result = Qt::AlignCenter;
    } else if (role == Qt::FontRole && index.row() < 6 && index.column() < 7) {
		// Выделяем текущий день
        Day day = days[index.row()][index.column()];
        QDate dt(QDate::currentDate().year(), day.month, day.mday);
		if (dt == QDate::currentDate()) {
			QFont font;
			font.setBold(true);
			result = font;
		}
    } else if (role == Qt::BackgroundColorRole && index.row() < 6 	&& index.column() < 7) {
		// Подсвечиваем различные типы дней
        DayType t = days[index.row()][index.column()].type;
		switch (t) {
		case WORK:
			result = qVariantFromValue(QColor(255, 255, 255));
			break;
		case OVERTIME:
			result = qVariantFromValue(QColor(186, 186, 186));
			break;
		case RESTDAY:
			result = qVariantFromValue(QColor(255, 186, 186));
			break;
		case HOLIDAY:
			result = qVariantFromValue(QColor(180, 0, 0));
			break;
		case VACATION:
			result = qVariantFromValue(QColor(80, 255, 80));
			break;
		case ILLNESS:
			result = qVariantFromValue(QColor(255, 235, 17));
			break;
		case COMPENSATORY:
			result = qVariantFromValue(QColor(186, 255, 186));
			break;
        case ILLNESS_LIST:
            result = qVariantFromValue(QColor(255, 174, 0));
            break;
		case NONE:
			result = qVariantFromValue(QColor(255, 255, 255));
			break;
		}
	}
	return result;
}

QVariant DaysTableModel::headerData(int section, Qt::Orientation orient, int role) const {
	QVariant result;
	if (role == Qt::DisplayRole && orient == Qt::Horizontal) {
		switch (section) {
		case 0:
			result = QVariant(tr("Пн"));
			break;
		case 1:
			result = QVariant(tr("Вт"));
			break;
		case 2:
			result = QVariant(tr("Ср"));
			break;
		case 3:
			result = QVariant(tr("Чт"));
			break;
		case 4:
			result = QVariant(tr("Пт"));
			break;
		case 5:
			result = QVariant(tr("Сб"));
			break;
		case 6:
			result = QVariant(tr("Вс"));
			break;
		}
	}
	return result;
}

QModelIndex DaysTableModel::index(int row, int column, const QModelIndex& parent) const {
    return !parent.isValid() ? createIndex(row, column, const_cast<Day*>(&days[row][column]) ) : QModelIndex();
}

void DaysTableModel::init(int month) {
	QDate dt(QDate::currentDate().year(), month, 1);
	DaysHelper helper;
	int maxDays = dt.daysInMonth();
	int day = 1;
    beginResetModel();
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 7; j++) {
			QDate dt(QDate::currentDate().year(), month, day);
			int dw = dt.dayOfWeek();
			if (dw == j + 1 && day <= maxDays) {
                days[i][j] = Day(helper.getType(dt), dt.month(), dw, day);
				++day;
			} else {
                days[i][j] = Day();
			}
		}
	}
    endResetModel();
}

Day DaysTableModel::dayFromIndex(const QModelIndex& index) {
    if (index.isValid())
        return days[index.row()][index.column()];
    else
        return Day();
}
