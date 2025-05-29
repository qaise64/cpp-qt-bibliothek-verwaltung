#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class DatabaseManager {

public:

	DatabaseManager();
	bool openDatabase(const QString& dbName);
	bool createTables();
	bool addBook(const QString& title, const QString& author, int year, const QString& status, const QString& description, const QByteArray& imageData);

private:

	QSqlDatabase db; 

};