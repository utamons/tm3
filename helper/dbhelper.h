#pragma once

#include <QtSql>
#include <QMessageBox>
#include <QApplication>
#include <functional>

enum SqlQueryStatus {
    SQL_OK,
    FOREIGN_KEY_FAIL
};

/*
  DB related staff
 */
class DBHelper
{

public:
    QSqlDatabase db;

	// We use only one connection to DB.
    static DBHelper* getInstance()
    {
        if (!_self) {
            _self = new DBHelper();
        }
        return _self;
    }

    void close();

protected:
	static DBHelper* _self;
    DBHelper();

    void checkDB();
    void createStructure();
};

// Executes SQL query, which doesn't return result set.
SqlQueryStatus execQuery(const QString query, bool checkForeignKey = false);

// Executes prepared SQL query, which doesn't return result set.
SqlQueryStatus execQuery(QSqlQuery query, bool checkForeignKey = false);

// Performs function on every record
void execQuery(QSqlQuery q, std::function<void ()> f);

void beginTransaction();

void commit();

void rollback();

QSqlDatabase getDb();

// Gets field value
template <typename T>
T field(QSqlQuery q, const char *fieldName);
template <typename T>
T field(QSqlQuery q, const char *fieldName) {
    return qvariant_cast<T>(q.value(q.record().indexOf(fieldName)));
}

template <typename T>
T field(QSqlQuery q, int fldIdx);
template <typename T>
T field(QSqlQuery q, int fldIdx) {
    return qvariant_cast<T>(q.value(fldIdx));
}

template <typename T>
int maxDBRecords();
template <typename T>
int maxDBRecords() {
    QSqlQuery q(getDb());
    int result = 0;

    T::prepareDBMaxRecords(q);

    execQuery(q,[q,&result] {
        result = field<int>(q,0);
    });

    return result;
}

template <typename B, typename T>
bool isDBUnique(T entity);
template <typename B, typename T>
bool isDBUnique(T entity) {
    bool result = false;
    QSqlQuery q(getDb());

    B::prepareDBUnique(q,entity);

    execQuery(q, [q,&result] {
        result = field<int>(q, 0) == 0;
    });

    return result;
}
