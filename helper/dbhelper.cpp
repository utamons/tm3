#include <QApplication>
#include <QtCore>
#include <QtGui>
#include "dbhelper.h"
#include "tm3util.h"
#include <QMessageBox>

DBHelper* DBHelper::_self = nullptr;

DBHelper::DBHelper() {
	db = QSqlDatabase::addDatabase("QSQLITE");
	checkDB();
}

void DBHelper::close() {
	if (db.isOpen()) {
		db.close();
	}
}

// Проверяем наличие файла базы. Если его нет, создаём базу и её структуру.
void DBHelper::checkDB() {

	QString dbPath = getWorkPath().append(QDir::toNativeSeparators("/tm2db.sqlite"));
	QString dbBack = getWorkPath().append(QDir::toNativeSeparators("/tm2db.sqlite.bak"));

	QFile dbFile(dbPath);
	QFile dbFileBack(dbBack);
	if (dbFile.exists()) {
		if (dbFileBack.exists()) {
			dbFileBack.remove();
		}
		dbFile.copy(dbBack);
	}

	db.setDatabaseName(dbPath);
	if (db.open()) {
		createStructure();
	} else {
		QString errorMsg = "Cannot open database:";
		errorMsg = errorMsg.append(db.lastError().text());
		qDebug() << errorMsg;
		QMessageBox::critical(nullptr, QApplication::tr("Ошибка базы данных"), errorMsg);
		QApplication::exit(1);
	}
	execQuery("PRAGMA foreign_keys = ON");
}

/*
 Создаём структуру. Если таблицы уже есть - они не создаются. Так что если изменилась
 структура таблицы - надо удалять файл базы.
 */
void DBHelper::createStructure() {

	//  Справочник категорий
	const QString catTableSQL = "create table if not exists cat_rb ("
								"id             integer primary key autoincrement," // Первичный ключ
			"abbrev         char(20) not null unique,"// Аббревиатура
			"parent_id      integer,"// Ссылка на родительский узел
			"virtual        integer not null check(virtual=0 or virtual=1),"// 1 - виртуальная категория (просто каталог), 0 - обычная категория, можно использовать
			"name           varchar(80) not null unique,"// Наименование
			"expanded       integer not null check(expanded=0 or expanded=1),"// Развернут или нет узел дерева
			"comment		varchar(255)"// Комментарий
			")";

	// Бюджеты
	const QString budjetSQL = "create table if not exists budjet ("
							  "id             integer primary key autoincrement," // Первичный ключ
			"name           varchar(80) not null,"// Наименование
			"workdays		integer check(workdays>-1 and workdays<2)," // Учитывать только рабочие дни (0-нет,1-да)
			"descendant		integer check(descendant>-1 and descendant<2)," // Учитывать потомков (0-нет,1-да)
			"illness		integer check(illness>-1 and illness<2)," // Учитывать дни болезни (0-нет,1-да)
			"correction		integer check(correction>-1 and correction<2)," // Требуется ли корректировка (0-нет,1-да)
			"min            integer,"// Минимальное значение
			"max            integer,"// Максимальное значение
			"dt_beg			numeric not null,"// Начало действия(в минутах с epoch)
			"dt_end			numeric,"         // Конец действия(в минутах с epoch)
			"period         integer not null check(period>-1 and period<6),"// Период бюджетирования 0-день,1-неделя,2-месяц,3-квартал,4-год,5-рабочий-день
			"cat_id         integer references cat_rb(id),"// Ссылка на категорию
			"rate_id        integer references rate_rb(id),"// Ссылка на оценку
			"tag_id         integer references tag_rb(id),"// Ссылка на метку
			"constraint budjet_uk unique (name,dt_beg,dt_end,period)"
			")";

	// Справочник единиц измерения
	const QString unitRbSQL = "create table if not exists unit_rb ("
							  "id            integer primary key autoincrement," // Первичный ключ
			"name          varchar(50) not null unique"// Наименование
			")";

	// Справочник единиц измерения
	const QString tagRbSQL = "create table if not exists tag_rb ("
							 "id            integer primary key autoincrement," // Первичный ключ
			"name          varchar(50) not null unique"// Наименование
			")";

	// Связь событий и меток
	const QString actTagLinkSQL = "create table if not exists act_tag_link ("
								  "act_id        integer references actions (id) not null,"
								  "tag_id        integer references tag_rb (id) not null,"
								  "constraint    act_tag_link_pk primary key (act_id,tag_id))";

	// Связь категорий и меток по умолчанию
    const QString catTagLinkSQL = "create table if not exists cat_tag_link ("
								  "cat_id        integer references cat_rb (id) not null,"
								  "tag_id        integer references tag_rb (id) not null,"
								  "constraint    cat_tag_link_pk primary key (cat_id,tag_id))";

	// Справочник оценок
	const QString rateRbTableSQL = "create table if not exists rate_rb ("
								   "id            integer primary key autoincrement," // Первичный ключ
			"name          varchar(80) not null unique,"// Наименование
			"time          integer,"// Время
			"comment	   varchar(255),"// Комментарий
			"unit_id       integer references unit_rb (id)"// Ссылка на ед. изм.
			")";

	// Связь категорий и оценок (многие ко многим)
	const QString catsRatesLinkSQL = "create table if not exists cat_rate_link ("
									 "cat_id         integer references cat_rb (id) not null," // Ссылка на категорию
			"rate_id        integer references rate_rb (id) not null,"// Ссылка на оценку
			"default_value  number not null, "// Значение по умолчанию
			"constraint cat_rate_link_pk primary key (cat_id,rate_id))";

	// Журнал событий
	const QString actionsSQL = "create table if not exists actions ("
							   "id             integer primary key autoincrement," // Первичный ключ
			"time           numeric not null, "// Время начала в минутах с epoch
			"cat_id         integer references cat_rb (id) not null,"// Категория
			"mins           integer not null,"// Длительность в минутах
			"comment        varchar(1024)"// Комментарий
			")";

	// Связь событий и оценок
	const QString actRateLinkSQL = "CREATE TABLE  if not exists act_rate_link ("
								   "act_id  integer references actions (id) not null," // Ссылка на событие
			"rate_id integer references rate_rb (id) not null,"// Ссылка на оценку
			"value   number not null, "
			"constraint act_rate_link_pk primary key (act_id,rate_id)"
			")";

	execQuery(catTableSQL);
	execQuery(unitRbSQL);
	execQuery(rateRbTableSQL);
	execQuery(catsRatesLinkSQL);
	execQuery(actionsSQL);
	execQuery(actRateLinkSQL);
	execQuery(actTagLinkSQL);
	execQuery(budjetSQL);
    execQuery(tagRbSQL);
	execQuery(catTagLinkSQL);
}

// Запрос, не возвращающий результатов
SqlQueryStatus execQuery(const QString query, bool checkForeignKey) {
	QSqlQuery q;
	if (!q.exec(query)) {
		if (checkForeignKey && q.lastError().databaseText() == "FOREIGN KEY constraint failed") {
			return FOREIGN_KEY_FAIL;
		} else {
			qDebug() << q.lastError();
			QMessageBox::critical(nullptr, QObject::tr("Ошибка базы данных"), q.lastError().text());
			QApplication::exit(1);
		}
	}
	return SQL_OK;
}

// Выполнение подготовленного запроса
SqlQueryStatus execQuery(QSqlQuery q, bool checkForeignKey) {
	if (!q.exec()) {
		if (checkForeignKey && q.lastError().databaseText() == "FOREIGN KEY constraint failed") {
			return FOREIGN_KEY_FAIL;
		} else {
			qDebug() << q.lastError();
			QMessageBox::critical(nullptr, QObject::tr("Ошибка базы данных"), q.lastError().text());
			QApplication::exit(1);
		}
	}
	return SQL_OK;
}

QSqlDatabase getDb() {
	return DBHelper::getInstance()->db;
}

// Выполнение функции для каждой записи
void execQuery(QSqlQuery q, std::function<void()> f) {
	if (q.exec()) {
		while (q.next()) {
			f();
		}
	} else {
		qDebug() << q.lastError();
		QMessageBox::critical(nullptr, QObject::tr("Ошибка базы данных"), q.lastError().text());
		QApplication::exit(1);
	}
}


void beginTransaction() {
    const QString error = "Cannot begin transaction (database does not support it?)";
    if (!getDb().transaction()) {
        qDebug() << error;
        QMessageBox::critical(nullptr, QObject::tr("Ошибка базы данных"), error);
        QApplication::exit(1);
    }
}

void commit() {
    QSqlDatabase db = getDb();
    if (!db.commit()) {
        qDebug() << db.lastError();
        QMessageBox::critical(nullptr, QObject::tr("Ошибка базы данных"), db.lastError().text());
        QApplication::exit(1);
    }
}

void rollback() {
    QSqlDatabase db = getDb();
    if (!db.rollback()) {
        qDebug() << db.lastError();
        QMessageBox::critical(nullptr, QObject::tr("Ошибка базы данных"), db.lastError().text());
        QApplication::exit(1);
    }
}

