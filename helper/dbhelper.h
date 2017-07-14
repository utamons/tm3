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
  Всякая всячина, связанная с БД - объявления.
 */
class DBHelper
{

public:
    QSqlDatabase db;

    // Поскольку используем только одно соединение с базой, то класс у нас - синглетон.
    static DBHelper* getInstance()
    {
        if (!_self) {
            _self = new DBHelper();
        }
        return _self;
    }

    // закрывает соединение с базой
    void close();

protected:
    static DBHelper* _self; // екземпляр синглетона
    DBHelper();

    // Проверяем наличие файла базы. Если его нет, создаём базу и её структуру.
    void checkDB();
    void createStructure();
};

// Запрос, не возвращающий результатов
SqlQueryStatus execQuery(const QString query, bool checkForeignKey = false);

// Выполнение подготовленного запроса, не возвращающего результатов.
SqlQueryStatus execQuery(QSqlQuery query, bool checkForeignKey = false);

// Выполнение функции для каждой записи запроса
void execQuery(QSqlQuery q, std::function<void ()> f);

void beginTransaction();

void commit();

void rollback();

// Получаем текущий экземпляр БД
QSqlDatabase getDb();

// Шаблон для получения значения поля из выполненного запроса.
template <typename T>
T getField(QSqlQuery q, const char *fieldName);
template <typename T>
T getField(QSqlQuery q, const char *fieldName) {
    return qvariant_cast<T>(q.value(q.record().indexOf(fieldName)));
}

template <typename T>
T getField(QSqlQuery q, int fldIdx);
template <typename T>
T getField(QSqlQuery q, int fldIdx) {
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
        result = getField<int>(q,0);
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
        result = getField<int>(q, 0) == 0;
    });

    return result;
}
