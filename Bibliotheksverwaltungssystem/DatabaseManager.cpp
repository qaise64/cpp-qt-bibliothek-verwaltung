#include "DatabaseManager.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QByteArray>
#include <QDebug>

DatabaseManager::DatabaseManager() {
    // Konstruktor-Implementierung (ggf. Initialisierung)
}

bool DatabaseManager::openDatabase(const QString& dbName) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug() << "Error: Unable to open database" << dbName;
        return false;
    }
    return true;
}

bool DatabaseManager::createTables() {
    QSqlQuery query(db);

    // Users-Tabelle
    QString createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE,
            password TEXT,
            rolle TEXT
        )
    )";
    if (!query.exec(createUsersTable)) {
        qDebug() << "Error: Failed to create table 'users':" << query.lastError().text();
        return false;
    }


    QString createBooksTable = R"(
    CREATE TABLE IF NOT EXISTS books (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT,
        author TEXT,
        year INTEGER,
        status TEXT, -- 'verfügbar' oder 'ausgeliehen'
        description TEXT, -- NEU: Beschreibung
        image BLOB,  -- Bilddaten
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    if (!query.exec(createBooksTable)) {
        qDebug() << "Error: Failed to create table 'books':" << query.lastError().text();
        return false;
    }

    // Falls das Feld image noch nicht existiert (bei bestehender DB)
    query.exec("PRAGMA table_info(books)");
    bool hasImage = false;
    while (query.next()) {
        if (query.value(1).toString() == "image") {
            hasImage = true;
            break;
        }
    }
    if (!hasImage) {
        QSqlQuery alterQuery(db);
        if (!alterQuery.exec("ALTER TABLE books ADD COLUMN image BLOB")) {
            // Falls das Feld schon existiert, ignorieren
        }
    }

    // Ausleihen-Tabelle
    QString createLendingsTable = R"(
        CREATE TABLE IF NOT EXISTS lendings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER,
            book_id INTEGER,
            lend_date DATETIME,
            return_date DATETIME,
            returned INTEGER DEFAULT 0,
            FOREIGN KEY(user_id) REFERENCES users(id),
            FOREIGN KEY(book_id) REFERENCES books(id)
        )
    )";
    if (!query.exec(createLendingsTable)) {
        qDebug() << "Error: Failed to create table 'lendings':" << query.lastError().text();
        return false;
    }

    // --- Testdaten einfügen, falls noch nicht vorhanden ---

    // Benutzer: admin/admin (Bibliothekar), user/user (Benutzer)
    query.exec("INSERT OR IGNORE INTO users (username, password, rolle) VALUES ('admin', 'admin', 'Bibliothekar')");
    query.exec("INSERT OR IGNORE INTO users (username, password, rolle) VALUES ('user', 'user', 'Benutzer')");

    // Beispiel-Bücher mit/ohne Bild
    // Beispielbild laden (optional, falls vorhanden)
    QByteArray imageData;
    QFile imageFile("book_sample.jpg");
    if (imageFile.exists() && imageFile.open(QIODevice::ReadOnly)) {
        imageData = imageFile.readAll();
        imageFile.close();
    }

    // Buch mit Bild
    QSqlQuery insertBookQuery(db);
    insertBookQuery.prepare("INSERT OR IGNORE INTO books (title, author, year, status, image) VALUES (?, ?, ?, ?, ?)");
    insertBookQuery.addBindValue("Der Prozess");
    insertBookQuery.addBindValue("Franz Kafka");
    insertBookQuery.addBindValue(1925);
    insertBookQuery.addBindValue("verfügbar");
    insertBookQuery.addBindValue(imageData);
    insertBookQuery.exec();

    // Weitere Bücher ohne Bild
    query.exec("INSERT OR IGNORE INTO books (title, author, year, status) VALUES ('Faust', 'Johann Wolfgang von Goethe', 1808, 'verfügbar')");
    query.exec("INSERT OR IGNORE INTO books (title, author, year, status) VALUES ('Die Verwandlung', 'Franz Kafka', 1915, 'verfügbar')");
    query.exec("INSERT OR IGNORE INTO books (title, author, year, status) VALUES ('Effi Briest', 'Theodor Fontane', 1895, 'verfügbar')");

    // Beispiel-Ausleihe (optional)
    query.exec("SELECT id FROM users WHERE username='user'");
    int userId = -1;
    if (query.next()) userId = query.value(0).toInt();
    query.exec("SELECT id FROM books WHERE title='Faust'");
    int bookId = -1;
    if (query.next()) bookId = query.value(0).toInt();

    if (userId > 0 && bookId > 0) {
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM lendings WHERE user_id=? AND book_id=?");
        checkQuery.addBindValue(userId);
        checkQuery.addBindValue(bookId);
        if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() == 0) {
            QSqlQuery insertLending(db);
            insertLending.prepare("INSERT INTO lendings (user_id, book_id, lend_date, returned) VALUES (?, ?, datetime('now','-10 days'), 0)");
            insertLending.addBindValue(userId);
            insertLending.addBindValue(bookId);
            insertLending.exec();
        }
    }

    return true;
}

bool DatabaseManager::addBook(const QString& title, const QString& author, int year, const QString& status, const QString& description, const QByteArray& imageData) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO books (title, author, year, status, description, image) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(title);
    query.addBindValue(author);
    query.addBindValue(year);
    query.addBindValue(status);
    query.addBindValue(description);
    query.addBindValue(imageData);
    if (!query.exec()) {
        qDebug() << "Error: Failed to add book:" << query.lastError().text();
        return false;
    }
    return true;
}