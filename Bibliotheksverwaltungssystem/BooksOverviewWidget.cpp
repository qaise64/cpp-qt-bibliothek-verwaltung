#include "BooksOverviewWidget.h"
#include <QHeaderView>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QScroller>
#include <QFrame>
#include <DatabaseManager.h>

BooksOverviewWidget::BooksOverviewWidget(DatabaseManager* db, const QString& role, QWidget* parent)
    : QWidget(parent), db(db), role(role)
{
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Moderner Header-Bereich
    QHBoxLayout* headerLayout = new QHBoxLayout();

    headline = new QLabel("Bücherübersicht", this);
    headline->setObjectName("sectionTitleBooks");
    QFont f = headline->font();
    f.setPointSize(16);
    f.setBold(true);
    headline->setFont(f);

    // Modernes Suchfeld
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("🔍 Nach Titel oder Autor suchen...");
    searchEdit->setMinimumWidth(300);
    searchEdit->setObjectName("bookSearchBox");

    headerLayout->addWidget(headline);
    headerLayout->addStretch();
    headerLayout->addWidget(searchEdit);

    mainLayout->addLayout(headerLayout);

    // Trennlinie
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setObjectName("headerSeparator");
    mainLayout->addWidget(line);

    // Scrollbereich für die Buchkarten
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Container für die Bücherkarten mit Grid-Layout
    booksContainer = new QWidget();
    booksContainer->setObjectName("booksContainer");

    booksLayout = new QGridLayout(booksContainer);
    booksLayout->setContentsMargins(10, 0, 10, 0);
    booksLayout->setSpacing(20);

    scrollArea->setWidget(booksContainer);
    mainLayout->addWidget(scrollArea, 1); // Nimmt den ganzen verfügbaren Platz ein

    // Event-Verbindung für die Suche
    connect(searchEdit, &QLineEdit::textChanged, this, &BooksOverviewWidget::onSearchTextChanged);

    // Laden der Bücher
    reloadBooks();
}

void BooksOverviewWidget::reloadBooks(const QString& filter)
{
    // Bestehende Bücherkarten entfernen
    QLayoutItem* item;
    while ((item = booksLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    // Bücher aus der Datenbank laden
    QSqlQuery query(db->getDatabase());
    QString sql = "SELECT id, title, author, year, status, description, image FROM books";
    if (!filter.isEmpty()) {
        sql += " WHERE title LIKE ? OR author LIKE ?";
        query.prepare(sql);
        QString f = "%" + filter + "%";
        query.addBindValue(f);
        query.addBindValue(f);
    }
    else {
        query.prepare(sql);
    }

    if (!query.exec()) {
        qDebug() << "Fehler beim Laden der Bücher:" << query.lastError().text();
        return;
    }

    // Raster-Layout mit 3 Spalten für die Bücherkarten
    int row = 0;
    int col = 0;
    int maxColumns = 3;

    while (query.next()) {
        int bookId = query.value("id").toInt();
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        int year = query.value("year").toInt();
        QString status = query.value("status").toString();
        QString description = query.value("description").toString();
        QByteArray imageData = query.value("image").toByteArray();

        // Buchkarte erstellen
        QWidget* bookCard = createBookCard(bookId, title, author, year, status, description, imageData);

        // Karte zum Layout hinzufügen
        booksLayout->addWidget(bookCard, row, col);

        // Nächste Position im Raster berechnen
        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }

    // Wenn keine Bücher gefunden wurden
    if (row == 0 && col == 0) {
        QLabel* noResults = new QLabel("Keine Bücher gefunden.");
        noResults->setAlignment(Qt::AlignCenter);
        noResults->setObjectName("noResultsLabel");
        booksLayout->addWidget(noResults, 0, 0, 1, maxColumns, Qt::AlignCenter);
    }

    // Leere Zellen hinzufügen, um das Raster zu füllen
    while (col > 0 && col < maxColumns) {
        booksLayout->addItem(new QSpacerItem(0, 0), row, col);
        col++;
    }

    // Abstandshalter am Ende hinzufügen für richtige Ausrichtung
    booksLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding),
        row + 1, 0, 1, maxColumns);
}

QWidget* BooksOverviewWidget::createBookCard(int bookId, const QString& title, const QString& author,
    int year, const QString& status, const QString& description,
    const QByteArray& imageData)
{
    // Container für die Buchkarte
    QWidget* card = new QWidget();
    card->setObjectName("bookCard");
    card->setProperty("status", status);
    card->setMinimumWidth(300);
    card->setFixedHeight(220);

    // Layout für die Karte
    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(15, 15, 15, 15);

    // Buchcover
    QLabel* coverLabel = new QLabel();
    coverLabel->setFixedSize(130, 189);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setScaledContents(true);
    coverLabel->setObjectName("bookCover");

    if (!imageData.isEmpty()) {
        QPixmap pix;
        pix.loadFromData(imageData);
        coverLabel->setPixmap(pix.scaled(coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else {
        coverLabel->setText("Kein Bild");
    }

    // Rechte Seite mit Informationen und Buttons
    QWidget* infoWidget = new QWidget();
    QVBoxLayout* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);

    // Titel
    QLabel* titleLabel = new QLabel(title);
    titleLabel->setObjectName("bookTitle");
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 1);
    titleLabel->setFont(titleFont);
    titleLabel->setWordWrap(true);

    // Autor
    QLabel* authorLabel = new QLabel(author);
    authorLabel->setObjectName("bookAuthor");

    // Jahr & Status in einer Zeile
    QWidget* metaWidget = new QWidget();
    QHBoxLayout* metaLayout = new QHBoxLayout(metaWidget);
    metaLayout->setContentsMargins(0, 0, 0, 0);
    metaLayout->setSpacing(10);

    QLabel* yearLabel = new QLabel(QString::number(year));
    yearLabel->setObjectName("bookYear");

    QLabel* statusLabel = new QLabel(status);
    statusLabel->setObjectName(status == "verfügbar" ? "statusAvailable" : "statusUnavailable");

    metaLayout->addWidget(yearLabel);
    metaLayout->addWidget(statusLabel);
    metaLayout->addStretch();

    // Beschreibung (gekürzt wenn zu lang)
    QString shortDesc = description;
    if (shortDesc.length() > 80) {
        shortDesc = shortDesc.left(80) + "...";
    }
    QLabel* descLabel = new QLabel(shortDesc);
    descLabel->setObjectName("bookDescription");
    descLabel->setWordWrap(true);

    // Alles zum Info-Layout hinzufügen
    infoLayout->addWidget(titleLabel);
    infoLayout->addWidget(authorLabel);
    infoLayout->addWidget(metaWidget);
    infoLayout->addWidget(descLabel);
    infoLayout->addStretch();


    // Button-Bereich für alle Benutzer (statt nur für Bibliothekare)

    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(10);
    actionLayout->setContentsMargins(0, 10, 0, 0);

    if (role == "Bibliothekar") {
        // Buttons für Bibliothekare
        QPushButton* editBtn = new QPushButton("Bearbeiten");
        editBtn->setProperty("bookId", bookId);
        editBtn->setProperty("action", "edit");
        editBtn->setObjectName("editButton");
        editBtn->setFixedHeight(32);
        connect(editBtn, &QPushButton::clicked, this, &BooksOverviewWidget::onActionClicked);

        QPushButton* delBtn = new QPushButton("Löschen");
        delBtn->setProperty("bookId", bookId);
        delBtn->setProperty("action", "delete");
        delBtn->setObjectName("deleteButton");
        delBtn->setFixedHeight(32);
        connect(delBtn, &QPushButton::clicked, this, &BooksOverviewWidget::onActionClicked);

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(delBtn);
    }
    else {
        // Button für normale Benutzer
        if (status == "verfügbar") {
            QPushButton* requestBtn = new QPushButton("Ausleihen beantragen");
            requestBtn->setProperty("bookId", bookId);
            requestBtn->setProperty("action", "request");
            requestBtn->setObjectName("requestButton");
            requestBtn->setFixedHeight(32);
            requestBtn->setMinimumWidth(150); 
            connect(requestBtn, &QPushButton::clicked, this, &BooksOverviewWidget::onActionClicked);
            actionLayout->addWidget(requestBtn);
        }
        else {
            // Info anzeigen, wenn bereits ausgeliehen
            QLabel* unavailableLabel = new QLabel("Nicht verfügbar");
            unavailableLabel->setObjectName("unavailableLabel");
            actionLayout->addWidget(unavailableLabel);
        }
    }

    actionLayout->addStretch();
    infoLayout->addLayout(actionLayout);

    // Cover und Info zum Karten-Layout hinzufügen
    cardLayout->addWidget(coverLabel);
    cardLayout->addWidget(infoWidget, 1);

    return card;

}

void BooksOverviewWidget::onSearchTextChanged(const QString& text)
{
    reloadBooks(text);
}

void BooksOverviewWidget::onActionClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int bookId = btn->property("bookId").toInt();
    QString action = btn->property("action").toString();

    if (action == "edit") {
        emit editBook(bookId);
    }
    else if (action == "delete") {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Buch löschen",
            "Möchten Sie dieses Buch wirklich löschen?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (db->deleteBook(bookId)) {
                QMessageBox::information(this, "Erfolg", "Buch wurde erfolgreich gelöscht.");
                reloadBooks(searchEdit->text());
            }
            else {
                QMessageBox::warning(this, "Fehler", "Buch konnte nicht gelöscht werden.");
            }
        }
    }
    else if (action == "request") {
        // Ausleihanfrage erstellen
        if (db->createLendingRequest(bookId)) {
            QMessageBox::information(this, "Erfolg",
                "Deine Anfrage wurde erfolgreich erstellt. Ein Bibliothekar wird sie bald bearbeiten.");
            reloadBooks(searchEdit->text());
        }
        else {
            QMessageBox::warning(this, "Fehler", "Ausleihanfrage konnte nicht erstellt werden.");
        }
    }
}