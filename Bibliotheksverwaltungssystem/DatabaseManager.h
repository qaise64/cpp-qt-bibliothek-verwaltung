#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class DatabaseManager {

public:

	DatabaseManager();
	bool openDatabase(const QString& dbName);
	bool createTables();

private:

	QSqlDatabase db; 

};