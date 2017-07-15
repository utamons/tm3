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

// Checking for database file. Creating this if needed.
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
		QMessageBox::critical(nullptr, QApplication::tr("Database error"), errorMsg);
		QApplication::exit(1);
	}
	execQuery("PRAGMA foreign_keys = ON");
}


// Creating database structure.
void DBHelper::createStructure() {

	//  Categories
	const QString catTableSQL = "create table if not exists cat_rb ("
	                            "id             integer primary key autoincrement," // Primary key
	        "abbrev         char(20) not null unique,"// Abbreviation
			"parent_id      integer,"// Ссылка на родительский узел
	        "virtual        integer not null check(virtual=0 or virtual=1),"// 1 - virtual category, 0 - arbitrary category
	        "name           varchar(80) not null unique,"
	        "expanded       integer not null check(expanded=0 or expanded=1),"// Expanded in the tree view
	        "comment		varchar(255)"
			")";

	// Budgets
	const QString budjetSQL = "create table if not exists budjet ("
	                          "id             integer primary key autoincrement,"
	        "name           varchar(80) not null,"
	        "workdays		integer check(workdays>-1 and workdays<2)," // Count workdays only (0-no,1-yes)
	        "descendant		integer check(descendant>-1 and descendant<2)," // Count descendants (0-no,1-yes)
	        "illness		integer check(illness>-1 and illness<2)," // Count illness days (0-no,1-yes)
	        "correction		integer check(correction>-1 and correction<2)," // Need corrections (0-no,1-yes)
	        "min            integer,"// min limit
	        "max            integer,"// max limit
	        "dt_beg			numeric not null,"// Active since (minutes from epoch)
	        "dt_end			numeric,"         // Inactive since (minutes from epoch)
	        "period         integer not null check(period>-1 and period<6),"// Budjet period 0-day,1-week,2-month,3-quarter,4-year,5-workday
	        "cat_id         integer references cat_rb(id),"// Category fk
	        "rate_id        integer references rate_rb(id),"//Rate fk
	        "tag_id         integer references tag_rb(id),"// Tag fk
			"constraint budjet_uk unique (name,dt_beg,dt_end,period)"
			")";

	// Units
	const QString unitRbSQL = "create table if not exists unit_rb ("
	                          "id            integer primary key autoincrement,"
	        "name          varchar(50) not null unique"
			")";

	// Tags
	const QString tagRbSQL = "create table if not exists tag_rb ("
	                         "id            integer primary key autoincrement,"
	        "name          varchar(50) not null unique"
			")";


	const QString actTagLinkSQL = "create table if not exists act_tag_link ("
								  "act_id        integer references actions (id) not null,"
								  "tag_id        integer references tag_rb (id) not null,"
								  "constraint    act_tag_link_pk primary key (act_id,tag_id))";

    const QString catTagLinkSQL = "create table if not exists cat_tag_link ("
								  "cat_id        integer references cat_rb (id) not null,"
								  "tag_id        integer references tag_rb (id) not null,"
								  "constraint    cat_tag_link_pk primary key (cat_id,tag_id))";

	// Rates
	const QString rateRbTableSQL = "create table if not exists rate_rb ("
	                               "id            integer primary key autoincrement,"
	        "name          varchar(80) not null unique,"
	        "time          integer,"
	        "comment	   varchar(255),"
	        "unit_id       integer references unit_rb (id)"
			")";

	const QString catsRatesLinkSQL = "create table if not exists cat_rate_link ("
	                                 "cat_id         integer references cat_rb (id) not null,"
	        "rate_id        integer references rate_rb (id) not null,"
	        "default_value  number not null, "
			"constraint cat_rate_link_pk primary key (cat_id,rate_id))";

	// Actions
	const QString actionsSQL = "create table if not exists actions ("
	                           "id             integer primary key autoincrement,"
	        "time           numeric not null, "// Start time
	        "cat_id         integer references cat_rb (id) not null,"
	        "mins           integer not null,"// Duration
	        "comment        varchar(1024)"
			")";


	const QString actRateLinkSQL = "CREATE TABLE  if not exists act_rate_link ("
	                               "act_id  integer references actions (id) not null,"
	        "rate_id integer references rate_rb (id) not null,"
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


SqlQueryStatus execQuery(const QString query, bool checkForeignKey) {
	QSqlQuery q;
	if (!q.exec(query)) {
		if (checkForeignKey && q.lastError().databaseText() == "FOREIGN KEY constraint failed") {
			return FOREIGN_KEY_FAIL;
		} else {
			qDebug() << q.lastError();
			QMessageBox::critical(nullptr, QObject::tr("Database error"), q.lastError().text());
			QApplication::exit(1);
		}
	}
	return SQL_OK;
}


SqlQueryStatus execQuery(QSqlQuery q, bool checkForeignKey) {
	if (!q.exec()) {
		if (checkForeignKey && q.lastError().databaseText() == "FOREIGN KEY constraint failed") {
			return FOREIGN_KEY_FAIL;
		} else {
			qDebug() << q.lastError();
			QMessageBox::critical(nullptr, QObject::tr("Database error"), q.lastError().text());
			QApplication::exit(1);
		}
	}
	return SQL_OK;
}

QSqlDatabase getDb() {
	return DBHelper::getInstance()->db;
}


void execQuery(QSqlQuery q, std::function<void()> f) {
	if (q.exec()) {
		while (q.next()) {
			f();
		}
	} else {
		qDebug() << q.lastError();
		QMessageBox::critical(nullptr, QObject::tr("Database error"), q.lastError().text());
		QApplication::exit(1);
	}
}


void beginTransaction() {
    const QString error = "Cannot begin transaction (database does not support it?)";
    if (!getDb().transaction()) {
        qDebug() << error;
		QMessageBox::critical(nullptr, QObject::tr("Database error"), error);
        QApplication::exit(1);
    }
}

void commit() {
    QSqlDatabase db = getDb();
    if (!db.commit()) {
        qDebug() << db.lastError();
		QMessageBox::critical(nullptr, QObject::tr("Database error"), db.lastError().text());
        QApplication::exit(1);
    }
}

void rollback() {
    QSqlDatabase db = getDb();
    if (!db.rollback()) {
        qDebug() << db.lastError();
		QMessageBox::critical(nullptr, QObject::tr("Database error"), db.lastError().text());
        QApplication::exit(1);
    }
}

