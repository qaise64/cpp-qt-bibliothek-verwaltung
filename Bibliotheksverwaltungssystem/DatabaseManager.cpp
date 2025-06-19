#include "DatabaseManager.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QTime>
#include <QDateTime>

DatabaseManager::DatabaseManager() {
   
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

    // Books-Tabelle mit ALLEN benötigten Spalten
    QString createBooksTable = R"(
    CREATE TABLE IF NOT EXISTS books (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT,
        author TEXT,
        year INTEGER,
        status TEXT, -- 'verfügbar' oder 'ausgeliehen'
        description TEXT, -- Beschreibung
        image BLOB,  -- Bilddaten
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    if (!query.exec(createBooksTable)) {
        qDebug() << "Error: Failed to create table 'books':" << query.lastError().text();
        return false;
    }

    query.exec("PRAGMA table_info(books)");
    bool hasDescription = false;
    bool hasImage = false;

    while (query.next()) {
        QString columnName = query.value(1).toString();
        if (columnName == "description") {
            hasDescription = true;
        }
        else if (columnName == "image") {
            hasImage = true;
        }
    }

    if (!hasDescription) {
        QSqlQuery alterQuery(db);
        if (!alterQuery.exec("ALTER TABLE books ADD COLUMN description TEXT")) {
            qDebug() << "Error: Failed to add description column:" << alterQuery.lastError().text();
        }
    }

    if (!hasImage) {
        QSqlQuery alterQuery(db);
        if (!alterQuery.exec("ALTER TABLE books ADD COLUMN image BLOB")) {
            qDebug() << "Error: Failed to add image column:" << alterQuery.lastError().text();
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


    // Ausleihanfragen-Tabelle
    QString createLendingRequestsTable = R"(
    CREATE TABLE IF NOT EXISTS lending_requests (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        user_id INTEGER,
        book_id INTEGER,
        request_date DATETIME,
        status TEXT, -- 'pending', 'approved', 'rejected'
        FOREIGN KEY(user_id) REFERENCES users(id),
        FOREIGN KEY(book_id) REFERENCES books(id)
    )
)";
    if (!query.exec(createLendingRequestsTable)) {
        qDebug() << "Error: Failed to create table 'lending_requests':" << query.lastError().text();
        return false;
    }

    // Ausleihverwaltungs-Tabelle ändern/erstellen
    QString updateLendingsTable = R"(
    CREATE TABLE IF NOT EXISTS lendings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    book_id INTEGER,
    lend_date DATETIME,
    due_date DATETIME,  -- Fälligkeitsdatum
    return_date DATETIME,
    returned INTEGER DEFAULT 0,
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(book_id) REFERENCES books(id)
)
)";
    if (!query.exec(updateLendingsTable)) {
        qDebug() << "Error: Failed to create table 'lendings':" << query.lastError().text();
        return false;
    }

    // Prüfen und ggf. due_date-Spalte hinzufügen
    query.exec("PRAGMA table_info(lendings)");
    bool hasDueDate = false;
    bool hasConfirmedReturn = false;
    bool hasExtended = false;

    while (query.next()) {
        QString columnName = query.value(1).toString();
        if (columnName == "due_date") {
            hasDueDate = true;
        }
        else if (columnName == "confirmed_return") {
            hasConfirmedReturn = true;
        }
        else if (columnName == "extended") {
            hasExtended = true;
        }
    }

    if (!hasDueDate) {
        QSqlQuery alterQuery(db);
        if (!alterQuery.exec("ALTER TABLE lendings ADD COLUMN due_date DATETIME")) {
            qDebug() << "Error: Failed to add due_date column:" << alterQuery.lastError().text();
        }
    }

    if (!hasConfirmedReturn) {
        QSqlQuery alterQuery(db);
        if (!alterQuery.exec("ALTER TABLE lendings ADD COLUMN confirmed_return INTEGER DEFAULT 0")) {
            qDebug() << "Error: Failed to add confirmed_return column:" << alterQuery.lastError().text();
        }
    }

    if (!hasExtended) {
        QSqlQuery alterQuery(db);
        if (!alterQuery.exec("ALTER TABLE lendings ADD COLUMN extended INTEGER DEFAULT 0")) {
            qDebug() << "Error: Failed to add extended column:" << alterQuery.lastError().text();
        }
    }

    return true;
}

bool DatabaseManager::addBook(const QString& title, const QString& author, int year, const QString& status, const QString& description, const QByteArray& imageData) {
    // Sicherstellen, dass kein Parameter leer ist (außer Bild)
    if (title.isEmpty() || author.isEmpty() || status.isEmpty()) {
        qDebug() << "Error: Cannot add book with empty title, author or status";
        return false;
    }

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

    qDebug() << "Book added successfully: " << title << " by " << author;
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;

}

bool DatabaseManager::getBook(int bookId, QString& title, QString& author, int& year, QString& status, QString& description, QByteArray& imageData) {
    QSqlQuery query(db);
    query.prepare("SELECT title, author, year, status, description, image FROM books WHERE id = ?");
    query.addBindValue(bookId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Error: Failed to get book:" << query.lastError().text();
        return false;
    }

    title = query.value("title").toString();
    author = query.value("author").toString();
    year = query.value("year").toInt();
    status = query.value("status").toString();
    description = query.value("description").toString();
    imageData = query.value("image").toByteArray();

    return true;
}

bool DatabaseManager::updateBook(int bookId, const QString& title, const QString& author, int year, const QString& status, const QString& description, const QByteArray& imageData) {
    // Sicherstellen, dass kein Parameter leer ist (außer Bild)
    if (title.isEmpty() || author.isEmpty() || status.isEmpty()) {
        qDebug() << "Error: Cannot update book with empty title, author or status";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE books SET title = ?, author = ?, year = ?, status = ?, description = ?, image = ? WHERE id = ?");
    query.addBindValue(title);
    query.addBindValue(author);
    query.addBindValue(year);
    query.addBindValue(status);
    query.addBindValue(description);
    query.addBindValue(imageData);
    query.addBindValue(bookId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to update book:" << query.lastError().text();
        return false;
    }

    qDebug() << "Book updated successfully: " << title << " by " << author;
    return true;
}

bool DatabaseManager::deleteBook(int bookId) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM books WHERE id = ?");
    query.addBindValue(bookId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to delete book:" << query.lastError().text();
        return false;
    }

    qDebug() << "Book deleted successfully, ID: " << bookId;
    return true;
}



QVector<QHash<QString, QVariant>> DatabaseManager::getLendingRequests() {
    QVector<QHash<QString, QVariant>> requests;

    QSqlQuery query(db);
    query.prepare("SELECT lr.id, lr.user_id, lr.book_id, lr.request_date, lr.status, "
        "u.username, b.title, b.author "
        "FROM lending_requests lr "
        "JOIN users u ON lr.user_id = u.id "
        "JOIN books b ON lr.book_id = b.id "
        // Status 'pending', 'approved' oder 'rejected' anzeigen, anstatt nur 'pending'
        "ORDER BY lr.request_date DESC");

    if (!query.exec()) {
        qDebug() << "Error: Konnte Ausleihanfragen nicht abrufen:" << query.lastError().text();
        return requests;
    }

    while (query.next()) {
        QHash<QString, QVariant> request;
        request["id"] = query.value("id");
        request["user_id"] = query.value("user_id");
        request["book_id"] = query.value("book_id");
        request["request_date"] = query.value("request_date");
        request["status"] = query.value("status");
        request["username"] = query.value("username");
        request["title"] = query.value("title");
        request["author"] = query.value("author");
        requests.append(request);
    }

    return requests;
}


bool DatabaseManager::approveLendingRequest(int requestId, int lendDays) {
    // Transaktion starten für Atomarität
    db.transaction();

    // Überprüfen, ob die Anfrage existiert und noch ausstehend ist
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT user_id, book_id, status FROM lending_requests WHERE id = ?");
    checkQuery.addBindValue(requestId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        qDebug() << "Error: Ausleihanfrage nicht gefunden:" << checkQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Überprüfen, ob die Anfrage im Status "pending" ist
    QString status = checkQuery.value("status").toString();
    if (status != "pending") {
        qDebug() << "Error: Ausleihanfrage ist nicht mehr ausstehend. Aktueller Status:" << status;
        db.rollback();
        return false;
    }

    int userId = checkQuery.value(0).toInt();
    int bookId = checkQuery.value(1).toInt();

    // Überprüfen, ob das Buch existiert und den richtigen Status hat
    QSqlQuery bookQuery(db);
    bookQuery.prepare("SELECT status FROM books WHERE id = ?");
    bookQuery.addBindValue(bookId);

    if (!bookQuery.exec() || !bookQuery.next()) {
        qDebug() << "Error: Buch nicht gefunden:" << bookQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Aktuelle Zeit + lendDays für due_date berechnen
    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueDate = now.addDays(lendDays);

    // Ausleihe erstellen mit korrektem Datumsformat
    QSqlQuery createLendingQuery(db);
    createLendingQuery.prepare("INSERT INTO lendings (user_id, book_id, lend_date, due_date, returned) "
        "VALUES (?, ?, ?, ?, 0)");
    createLendingQuery.addBindValue(userId);
    createLendingQuery.addBindValue(bookId);
    createLendingQuery.addBindValue(now.toString("yyyy-MM-dd hh:mm:ss"));
    createLendingQuery.addBindValue(dueDate.toString("yyyy-MM-dd hh:mm:ss"));

    if (!createLendingQuery.exec()) {
        qDebug() << "Error: Ausleihe konnte nicht erstellt werden:" << createLendingQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Status der Anfrage aktualisieren
    QSqlQuery updateRequestQuery(db);
    updateRequestQuery.prepare("UPDATE lending_requests SET status = 'approved' WHERE id = ?");
    updateRequestQuery.addBindValue(requestId);

    if (!updateRequestQuery.exec()) {
        qDebug() << "Error: Anfragestatus konnte nicht aktualisiert werden:" << updateRequestQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Buch-Status aktualisieren
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'ausgeliehen' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);

    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Buchstatus konnte nicht aktualisiert werden:" << updateBookQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Transaktion abschließen
    if (!db.commit()) {
        qDebug() << "Error: Transaktion konnte nicht abgeschlossen werden:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "Ausleihanfrage erfolgreich genehmigt, Buch ID:" << bookId << ", Nutzer ID:" << userId;
    return true;
}

bool DatabaseManager::rejectLendingRequest(int requestId) {

    // Überprüfen, ob die Anfrage existiert und im Status "pending" ist
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT book_id, status FROM lending_requests WHERE id = ?");
    checkQuery.addBindValue(requestId);
    
    if (!checkQuery.exec() || !checkQuery.next()) {
        qDebug() << "Error: Ausleihanfrage nicht gefunden:" << checkQuery.lastError().text();
        return false;
    }
    
    QString status = checkQuery.value("status").toString();
    if (status != "pending") {
        qDebug() << "Error: Ausleihanfrage ist nicht mehr ausstehend. Aktueller Status:" << status;
        return false;
    }
    
    int bookId = checkQuery.value(0).toInt();
    
    // Status auf "rejected" setzen
    QSqlQuery updateRequestQuery(db);
    updateRequestQuery.prepare("UPDATE lending_requests SET status = 'rejected' WHERE id = ?");
    updateRequestQuery.addBindValue(requestId);
    
    if (!updateRequestQuery.exec()) {
        qDebug() << "Error: Anfragestatus konnte nicht aktualisiert werden:" << updateRequestQuery.lastError().text();
        return false;
    }
    
    // Buch-Status zurück auf "verfügbar" setzen
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'verfügbar' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);
    
    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Buchstatus konnte nicht aktualisiert werden:" << updateBookQuery.lastError().text();
        return false;
    }
    
    return true;
}


bool DatabaseManager::returnLendingRequest(int requestId) {
    // Anfrage-Details abrufen
    QSqlQuery getRequestQuery(db);
    getRequestQuery.prepare("SELECT book_id, status FROM lending_requests WHERE id = ?");
    getRequestQuery.addBindValue(requestId);

    if (!getRequestQuery.exec() || !getRequestQuery.next()) {
        qDebug() << "Error: Ausleihanfrage nicht gefunden:" << getRequestQuery.lastError().text();
        return false;
    }

    int bookId = getRequestQuery.value(0).toInt();
    QString status = getRequestQuery.value(1).toString();

    // Nur genehmigte Anfragen können zurückgegeben werden
    if (status != "approved") {
        qDebug() << "Error: Nur genehmigte Anfragen können zurückgegeben werden. Aktueller Status:" << status;
        return false;
    }

    // Status auf "returned" setzen (wir könnten auch einen neuen Status einführen)
    QSqlQuery updateRequestQuery(db);
    updateRequestQuery.prepare("UPDATE lending_requests SET status = 'returned' WHERE id = ?");
    updateRequestQuery.addBindValue(requestId);

    if (!updateRequestQuery.exec()) {
        qDebug() << "Error: Anfragestatus konnte nicht aktualisiert werden:" << updateRequestQuery.lastError().text();
        return false;
    }

    // Buch-Status auf "verfügbar" setzen
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'verfügbar' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);

    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Buchstatus konnte nicht aktualisiert werden:" << updateBookQuery.lastError().text();
        return false;
    }

    qDebug() << "Ausleihanfrage erfolgreich abgeschlossen, Buch ID:" << bookId;
    return true;
}


QVector<QHash<QString, QVariant>> DatabaseManager::getUserLendings(int userId) {
    QVector<QHash<QString, QVariant>> lendings;

    // Get user ID if not provided
    if (userId == -1) {
        QSqlQuery userQuery(db);
        userQuery.prepare("SELECT id FROM users WHERE username = 'user'");
        if (userQuery.exec() && userQuery.next()) {
            userId = userQuery.value(0).toInt();
        }
        else {
            return lendings;
        }
    }

    // Get user's actual lendings
    QSqlQuery lendingsQuery(db);
    lendingsQuery.prepare(
        "SELECT l.id, l.book_id, l.lend_date, l.due_date, l.returned, "
        "IFNULL(l.confirmed_return, 0) as confirmed_return, "
        "IFNULL(l.extended, 0) as extended, "
        "b.title, b.author "
        "FROM lendings l "
        "JOIN books b ON l.book_id = b.id "
        "WHERE l.user_id = ? "
        "ORDER BY l.lend_date DESC"
    );
    lendingsQuery.addBindValue(userId);

    if (lendingsQuery.exec()) {
        while (lendingsQuery.next()) {
            QHash<QString, QVariant> lending;
            lending["id"] = lendingsQuery.value("id");
            lending["book_id"] = lendingsQuery.value("book_id");
            lending["lend_date"] = lendingsQuery.value("lend_date");
            lending["request_date"] = lendingsQuery.value("lend_date"); // Für konsistente Sortierung
            lending["due_date"] = lendingsQuery.value("due_date");
            lending["returned"] = lendingsQuery.value("returned");
            lending["confirmed_return"] = lendingsQuery.value("confirmed_return");
            lending["extended"] = lendingsQuery.value("extended");
            lending["title"] = lendingsQuery.value("title");
            lending["author"] = lendingsQuery.value("author");
            lending["type"] = "lending";
            lendings.append(lending);
        }
    }

    // Get pending, rejected, or canceled requests (excluding approved)
    QSqlQuery requestsQuery(db);
    requestsQuery.prepare(
        "SELECT lr.id as request_id, lr.book_id, lr.request_date, lr.status, "
        "b.title, b.author "
        "FROM lending_requests lr "
        "JOIN books b ON lr.book_id = b.id "
        "WHERE lr.user_id = ? AND lr.status IN ('pending', 'rejected', 'canceled') "
        "ORDER BY lr.request_date DESC"
    );
    requestsQuery.addBindValue(userId);

    if (requestsQuery.exec()) {
        while (requestsQuery.next()) {
            QHash<QString, QVariant> request;
            request["request_id"] = requestsQuery.value("request_id");
            request["book_id"] = requestsQuery.value("book_id");
            request["lend_date"] = requestsQuery.value("request_date");
            request["request_date"] = requestsQuery.value("request_date"); // Für konsistente Sortierung
            request["title"] = requestsQuery.value("title");
            request["author"] = requestsQuery.value("author");
            request["status"] = requestsQuery.value("status");
            request["type"] = "request";
            lendings.append(request);
        }
    }

    // Sortieren nach Antragszeit absteigend (neueste zuerst)
    std::sort(lendings.begin(), lendings.end(), [](const QHash<QString, QVariant>& a, const QHash<QString, QVariant>& b) {
        return a["request_date"].toDateTime() > b["request_date"].toDateTime();
        });

    return lendings;
}


bool DatabaseManager::createLendingRequest(int bookId, int userId) {
    // Wenn kein Benutzer angegeben, aktuellen Benutzer nehmen
    if (userId == -1) {
        QSqlQuery userQuery(db);
        userQuery.prepare("SELECT id FROM users WHERE username = 'user'");
        if (userQuery.exec() && userQuery.next()) {
            userId = userQuery.value(0).toInt();
        }
        else {
            qDebug() << "Error: Benutzer-ID konnte nicht ermittelt werden";
            return false;
        }
    }

    // Prüfen, ob das Buch verfügbar ist
    QSqlQuery bookQuery(db);
    bookQuery.prepare("SELECT status FROM books WHERE id = ?");
    bookQuery.addBindValue(bookId);

    if (!bookQuery.exec() || !bookQuery.next()) {
        qDebug() << "Error: Buch nicht gefunden:" << bookQuery.lastError().text();
        return false;
    }

    QString status = bookQuery.value(0).toString();
    if (status != "verfügbar") {
        qDebug() << "Error: Buch ist nicht für Ausleihe verfügbar";
        return false;
    }

    // Prüfen, ob der Benutzer bereits eine Anfrage für dieses Buch hat
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT id FROM lending_requests WHERE user_id = ? AND book_id = ? AND status = 'pending'");
    checkQuery.addBindValue(userId);
    checkQuery.addBindValue(bookId);

    if (checkQuery.exec() && checkQuery.next()) {
        qDebug() << "Error: Benutzer hat bereits eine ausstehende Anfrage für dieses Buch";
        return false;
    }

    // Ausleihanfrage erstellen
    QSqlQuery query(db);
    query.prepare("INSERT INTO lending_requests (user_id, book_id, request_date, status) "
        "VALUES (?, ?, datetime('now'), 'pending')");
    query.addBindValue(userId);
    query.addBindValue(bookId);

    if (!query.exec()) {
        qDebug() << "Error: Ausleihanfrage konnte nicht erstellt werden:" << query.lastError().text();
        return false;
    }

    // Buch-Status auf 'angefragt' ändern
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'angefragt' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);

    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Buchstatus konnte nicht aktualisiert werden:" << updateBookQuery.lastError().text();
        return false;
    }

    return true;
}



bool DatabaseManager::confirmBookReturn(int lendingId) {
    // Transaktion starten für Atomarität
    db.transaction();

    // Prüfen, ob die Ausleihe existiert und im Status "returned=1" ist
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT book_id FROM lendings WHERE id = ? AND returned = 1 AND return_date IS NOT NULL");
    checkQuery.addBindValue(lendingId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        qDebug() << "Error: Ausleihe nicht gefunden oder noch nicht zurückgegeben:" << checkQuery.lastError().text();
        db.rollback();
        return false;
    }

    int bookId = checkQuery.value("book_id").toInt();

    // Buch-Status auf "verfügbar" setzen (falls es noch nicht der Fall ist)
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'verfügbar' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);

    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Buchstatus konnte nicht aktualisiert werden:" << updateBookQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Ausleihe als bestätigt markieren (z.B. mit einem neuen Feld)
    QSqlQuery updateLendingQuery(db);
    updateLendingQuery.prepare("UPDATE lendings SET confirmed_return = 1 WHERE id = ?");
    updateLendingQuery.addBindValue(lendingId);

    if (!updateLendingQuery.exec()) {
        qDebug() << "Error: Ausleihe konnte nicht bestätigt werden:" << updateLendingQuery.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qDebug() << "Error: Transaktion konnte nicht abgeschlossen werden:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "Buch-Rückgabe erfolgreich bestätigt, ID:" << bookId;
    return true;
}

bool DatabaseManager::extendLending(int lendingId, int additionalDays) {
    // Sicherstellung, dass die maximale Verlängerung nicht überschritten wird
    additionalDays = qMin(additionalDays, 10);

    // Transaktion starten
    db.transaction();

    // Prüfen, ob die Ausleihe existiert und noch nicht zurückgegeben wurde
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT due_date, IFNULL(extended, 0) as extended FROM lendings WHERE id = ? AND returned = 0");
    checkQuery.addBindValue(lendingId);

    if (!checkQuery.exec() || !checkQuery.next()) {
        qDebug() << "Error: Ausleihe nicht gefunden oder bereits zurückgegeben:" << checkQuery.lastError().text();
        db.rollback();
        return false;
    }

    // Prüfen, ob die Ausleihe bereits verlängert wurde
    bool extended = checkQuery.value("extended").toBool();
    if (extended) {
        qDebug() << "Error: Ausleihe wurde bereits verlängert.";
        db.rollback();
        return false;
    }

    // Aktuelles Fälligkeitsdatum abrufen und verlängern
    QDateTime dueDate = QDateTime::fromString(checkQuery.value("due_date").toString(), "yyyy-MM-dd hh:mm:ss");
    QDateTime newDueDate = dueDate.addDays(additionalDays);

    // Ausleihe aktualisieren
    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE lendings SET due_date = ?, extended = 1 WHERE id = ?");
    updateQuery.addBindValue(newDueDate.toString("yyyy-MM-dd hh:mm:ss"));
    updateQuery.addBindValue(lendingId);

    if (!updateQuery.exec()) {
        qDebug() << "Error: Ausleihe konnte nicht verlängert werden:" << updateQuery.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qDebug() << "Error: Transaktion konnte nicht abgeschlossen werden:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "Ausleihe erfolgreich verlängert, neue Fälligkeit:" << newDueDate.toString();
    return true;
}

QVector<QHash<QString, QVariant>> DatabaseManager::getPendingReturns() {
    QVector<QHash<QString, QVariant>> pendingReturns;

    QSqlQuery query(db);
    query.prepare(
        "SELECT l.id, l.book_id, l.user_id, l.lend_date, l.return_date, "
        "IFNULL(l.confirmed_return, 0) as confirmed_return, "  
        "b.title, b.author, u.username "
        "FROM lendings l "
        "JOIN books b ON l.book_id = b.id "
        "JOIN users u ON l.user_id = u.id "
        "WHERE l.returned = 1 " 
        "ORDER BY l.return_date DESC"
    );

    if (!query.exec()) {
        qDebug() << "Error: Konnte Rückgaben nicht abrufen:" << query.lastError().text();
        return pendingReturns;
    }

    while (query.next()) {
        QHash<QString, QVariant> pendingReturn;
        pendingReturn["id"] = query.value("id");
        pendingReturn["book_id"] = query.value("book_id");
        pendingReturn["user_id"] = query.value("user_id");
        pendingReturn["lend_date"] = query.value("lend_date");
        pendingReturn["return_date"] = query.value("return_date");
        pendingReturn["confirmed_return"] = query.value("confirmed_return");
        pendingReturn["title"] = query.value("title");
        pendingReturn["author"] = query.value("author");
        pendingReturn["username"] = query.value("username");
        pendingReturns.append(pendingReturn);
    }

    return pendingReturns;
}

bool DatabaseManager::returnBook(int lendingId) {
    // Ausleih-Details abrufen
    QSqlQuery getLendingQuery(db);
    getLendingQuery.prepare("SELECT book_id FROM lendings WHERE id = ? AND returned = 0");
    getLendingQuery.addBindValue(lendingId);

    if (!getLendingQuery.exec() || !getLendingQuery.next()) {
        qDebug() << "Error: Ausleihe nicht gefunden oder bereits zurückgegeben:" << getLendingQuery.lastError().text();
        return false;
    }

    int bookId = getLendingQuery.value(0).toInt();

    // Ausleihe als zurückgegeben markieren (aber noch nicht bestätigt)
    QSqlQuery updateLendingQuery(db);
    updateLendingQuery.prepare("UPDATE lendings SET returned = 1, return_date = datetime('now'), confirmed_return = 0 WHERE id = ?");
    updateLendingQuery.addBindValue(lendingId);

    if (!updateLendingQuery.exec()) {
        qDebug() << "Error: Konnte Ausleihe nicht aktualisieren:" << updateLendingQuery.lastError().text();
        return false;
    }

    // Buch-Status auf "zurückgegeben, warte auf Bestätigung" setzen
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'zurückgegeben' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);

    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Konnte Buchstatus nicht aktualisieren:" << updateBookQuery.lastError().text();
        return false;
    }

    qDebug() << "Buch wurde zurückgegeben, wartet auf Bestätigung, ID:" << bookId;
    return true;
}

bool DatabaseManager::cancelLendingRequest(int requestId) {
    // Anfrage-Details abrufen
    QSqlQuery getRequestQuery(db);
    getRequestQuery.prepare("SELECT book_id, status FROM lending_requests WHERE id = ?");
    getRequestQuery.addBindValue(requestId);

    if (!getRequestQuery.exec() || !getRequestQuery.next()) {
        qDebug() << "Error: Ausleihanfrage nicht gefunden:" << getRequestQuery.lastError().text();
        return false;
    }

    int bookId = getRequestQuery.value(0).toInt();
    QString status = getRequestQuery.value(1).toString();

    // Nur ausstehende Anfragen können abgebrochen werden
    if (status != "pending") {
        qDebug() << "Error: Nur ausstehende Anfragen können abgebrochen werden. Aktueller Status:" << status;
        return false;
    }

    // Status auf "canceled" setzen 
    QSqlQuery updateRequestQuery(db);
    updateRequestQuery.prepare("UPDATE lending_requests SET status = 'canceled' WHERE id = ?");
    updateRequestQuery.addBindValue(requestId);

    if (!updateRequestQuery.exec()) {
        qDebug() << "Error: Anfragestatus konnte nicht aktualisiert werden:" << updateRequestQuery.lastError().text();
        return false;
    }

    // Buch-Status auf "verfügbar" setzen
    QSqlQuery updateBookQuery(db);
    updateBookQuery.prepare("UPDATE books SET status = 'verfügbar' WHERE id = ?");
    updateBookQuery.addBindValue(bookId);

    if (!updateBookQuery.exec()) {
        qDebug() << "Error: Buchstatus konnte nicht aktualisiert werden:" << updateBookQuery.lastError().text();
        return false;
    }

    qDebug() << "Ausleihanfrage erfolgreich abgebrochen, Buch ID:" << bookId;
    return true;
}

// Statistikmethoden

int DatabaseManager::getTotalBooks() {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM books");
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getAvailableBooks() {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM books WHERE status = 'verfügbar'");
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getLentBooks() {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM books WHERE status = 'ausgeliehen'");
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getTotalLendings() {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM lendings");
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}

double DatabaseManager::getAverageLendingDuration() {
    QSqlQuery query(db);
    query.prepare("SELECT AVG(julianday(return_date) - julianday(lend_date)) FROM lendings WHERE returned = 1 AND return_date IS NOT NULL");
    query.exec();
    return query.next() ? query.value(0).toDouble() : 0.0;
}