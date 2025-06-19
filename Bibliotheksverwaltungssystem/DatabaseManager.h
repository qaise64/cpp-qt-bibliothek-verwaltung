#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class DatabaseManager {

public:

    DatabaseManager();
    bool openDatabase(const QString& dbName);
    bool createTables();
    bool addBook(const QString& title, const QString& author, int year, const QString& status, const QString& description, const QByteArray& imageData);
    bool updateBook(int bookId, const QString& title, const QString& author, int year, const QString& status, const QString& description, const QByteArray& imageData);
    bool deleteBook(int bookId);
    bool getBook(int bookId, QString& title, QString& author, int& year, QString& status, QString& description, QByteArray& imageData);
    QSqlDatabase& getDatabase();

    bool createLendingRequest(int bookId, int userId = -1);
    bool approveLendingRequest(int requestId, int lendDays = 14);
    bool rejectLendingRequest(int requestId);
    bool returnBook(int lendingId);
    QVector<QHash<QString, QVariant>> getLendingRequests();
    QVector<QHash<QString, QVariant>> getUserLendings(int userId = -1);
    bool returnLendingRequest(int requestId);
    bool cancelLendingRequest(int requestId);

	// Statistik-Methoden

    int getTotalBooks();
    int getAvailableBooks();
    int getLentBooks();
    int getTotalLendings();
    double getAverageLendingDuration();

    
    bool confirmBookReturn(int lendingId);  // Für Bibliothekare, bestätigt Rückgabe
    bool extendLending(int lendingId, int additionalDays = 10); // Für Benutzer, verlängert Ausleihe
    QVector<QHash<QString, QVariant>> getPendingReturns(); // Zeigt alle zur Bestätigung ausstehenden Rückgaben

private:
    QSqlDatabase db;
};