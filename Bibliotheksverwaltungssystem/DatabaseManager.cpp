#include "DatabaseManager.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>


DatabaseManager::DatabaseManager() {
    // Konstruktor-Implementierung (ggf. Initialisierung)
}


bool DatabaseManager::openDatabase(const QString& dbName) {

	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbName);
	if(!db.open()) {
		qDebug() << "Error: Unable to open database" << dbName;
		return false;
	}
	return true;

}

bool DatabaseManager::createTables() {

    QSqlQuery query(db);

    QString createUsersTable = R"(CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT,
    password TEXT,
    rolle TEXT
    )";

    if (!query.exec(createUsersTable)) {
        qDebug() << "Error: Failed to create table 'Users':" << query.lastError().text();
        return false;
    }

    // Weitere Tabellen können hier analog erstellt werden

    return true;

}
