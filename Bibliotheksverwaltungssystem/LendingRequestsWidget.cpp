#include "LendingRequestsWidget.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollArea>

LendingRequestsWidget::LendingRequestsWidget(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), db(db)
{
    setupUI();
    refreshRequests();
    this->setObjectName("LendingRequestsWidget");


}

void LendingRequestsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* title = new QLabel("Ausleihanfragen verwalten", this);
    title->setObjectName("sectionTitle");
    QFont f = title->font();
    f.setPointSize(16);
    f.setBold(true);
    title->setFont(f);

    mainLayout->addWidget(title);

    // Tabs für verschiedene Funktionen
    tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("tabWidget");

    // Tab 1: Ausleihanfragen
    QWidget* requestsWidget = new QWidget(tabWidget);
    QVBoxLayout* requestsTabLayout = new QVBoxLayout(requestsWidget);
    requestsTabLayout->setContentsMargins(0, 0, 0, 0);
    requestsWidget->setObjectName("requestsTabWidget");
	requestsWidget->setStyleSheet("background-color: #ffffff;");

    // Tabelle für Anfragen (nur für Kompatibilität, bleibt versteckt)
    requestsTable = new QTableWidget(requestsWidget);
    requestsTable->setColumnCount(6);
    QStringList headers;
    headers << "Anfrage-ID" << "Benutzer" << "Buch" << "Autor" << "Datum" << "Aktionen";
    requestsTable->setHorizontalHeaderLabels(headers);
    requestsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    requestsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    requestsTable->setColumnWidth(0, 60);
    requestsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    requestsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    requestsTable->verticalHeader()->setDefaultSectionSize(44);
    requestsTable->verticalHeader()->setVisible(false);
    requestsTable->setAlternatingRowColors(true);
    requestsTable->hide();

    // Layout für Anfragen
    QWidget* requestsContainer = new QWidget(requestsWidget);
    requestsLayout = new QVBoxLayout(requestsContainer);
    requestsLayout->setContentsMargins(0, 0, 0, 0);
    requestsLayout->setSpacing(10);
    requestsContainer->setObjectName("requestsContainer");

    QScrollArea* requestsScrollArea = new QScrollArea(this);
    requestsScrollArea->setWidgetResizable(true);
    requestsScrollArea->setFrameShape(QFrame::NoFrame);
    requestsScrollArea->setWidget(requestsContainer);
    requestsTabLayout->addWidget(requestsScrollArea);

    // Tab 2: Ausstehende Rückgaben
    QWidget* returnsWidget = new QWidget(tabWidget);
    QVBoxLayout* returnsTabLayout = new QVBoxLayout(returnsWidget);
    returnsTabLayout->setContentsMargins(0, 0, 0, 0);
    returnsTabLayout->setSpacing(10);
    returnsWidget->setObjectName("returnsTabWidget");
	returnsWidget->setStyleSheet("background-color: #ffffff;");

    QWidget* returnsContainer = new QWidget(returnsWidget);
    returnsLayout = new QVBoxLayout(returnsContainer);
    returnsLayout->setContentsMargins(0, 0, 0, 0);
    returnsLayout->setSpacing(10);
    returnsContainer->setObjectName("returnsContainer");

    QScrollArea* returnsScrollArea = new QScrollArea(this);
    returnsScrollArea->setWidgetResizable(true);
    returnsScrollArea->setFrameShape(QFrame::NoFrame);
    returnsScrollArea->setWidget(returnsContainer);

    returnsTabLayout->addWidget(returnsScrollArea);

    // Tabs hinzufügen
    tabWidget->addTab(requestsWidget, "Ausleihanfragen");
    tabWidget->addTab(returnsWidget, "Ausstehende Rückgaben");

    mainLayout->addWidget(tabWidget, 1);

    // Verbindung für Tab-Wechsel
    connect(tabWidget, &QTabWidget::currentChanged, [this](int index) {
        if (index == 0) {
            refreshRequests();
        }
        else if (index == 1) {
            refreshReturnsList();
        }
        });
}

void LendingRequestsWidget::refreshReturnsList()
{
    // UI-Elemente löschen
    QLayoutItem* item;
    while ((item = returnsLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    auto returns = db->getPendingReturns();

    if (returns.isEmpty()) {
        QLabel* noReturnsLabel = new QLabel("Keine ausstehenden Rückgaben.", this);
        noReturnsLabel->setAlignment(Qt::AlignCenter);
        returnsLayout->addWidget(noReturnsLabel);
        return;
    }

    // Kopfzeile
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 5, 10, 5);

    QLabel* idHeader = new QLabel("ID", headerWidget);
    QLabel* userHeader = new QLabel("Benutzer", headerWidget);
    QLabel* titleHeader = new QLabel("Buchtitel", headerWidget);
    QLabel* lendDateHeader = new QLabel("Ausgeliehen am", headerWidget);
    QLabel* returnDateHeader = new QLabel("Zurückgegeben am", headerWidget);
    QLabel* actionsHeader = new QLabel("Aktionen", headerWidget);

    idHeader->setFixedWidth(60);
    userHeader->setFixedWidth(150);
    titleHeader->setFixedWidth(350);
    lendDateHeader->setFixedWidth(200);
    returnDateHeader->setFixedWidth(200);

    headerLayout->addWidget(idHeader);
    headerLayout->addWidget(userHeader);
    headerLayout->addWidget(titleHeader);
    headerLayout->addWidget(lendDateHeader);
    headerLayout->addWidget(returnDateHeader);
    headerLayout->addWidget(actionsHeader, 1);

    headerWidget->setObjectName("requestsHeader");
    returnsLayout->addWidget(headerWidget);

    // Rückgaben durchlaufen
    for (const auto& ret : returns) {
        QWidget* returnWidget = new QWidget(this);
        QHBoxLayout* returnLayout = new QHBoxLayout(returnWidget);
        returnLayout->setContentsMargins(10, 5, 10, 5);

        QLabel* idLabel = new QLabel(ret["id"].toString(), returnWidget);
        QLabel* userLabel = new QLabel(ret["username"].toString(), returnWidget);
        QLabel* titleLabel = new QLabel(ret["title"].toString(), returnWidget);
        QLabel* lendDateLabel = new QLabel(ret["lend_date"].toDateTime().toString("dd.MM.yyyy hh:mm"), returnWidget);
        QLabel* returnDateLabel = new QLabel(ret["return_date"].toDateTime().toString("dd.MM.yyyy hh:mm"), returnWidget);

        idLabel->setFixedWidth(60);
        userLabel->setFixedWidth(150);
        titleLabel->setFixedWidth(350);
        lendDateLabel->setFixedWidth(200);
        returnDateLabel->setFixedWidth(200);

        returnLayout->addWidget(idLabel);
        returnLayout->addWidget(userLabel);
        returnLayout->addWidget(titleLabel);
        returnLayout->addWidget(lendDateLabel);
        returnLayout->addWidget(returnDateLabel);

        // Aktionen/Status
        bool isConfirmed = ret["confirmed_return"].toBool();
        if (isConfirmed) {
            QLabel* statusLabel = new QLabel("Bestätigt", returnWidget);
            statusLabel->setObjectName("lendingstatusLabel");
            returnLayout->addWidget(statusLabel, 1);
        }
        else {
            QPushButton* confirmBtn = new QPushButton("Bestätigen", returnWidget);
            confirmBtn->setObjectName("actionButton");
            confirmBtn->setProperty("lendingId", ret["id"].toInt());
            confirmBtn->setFixedHeight(35);
            connect(confirmBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onConfirmReturnClicked);
            returnLayout->addWidget(confirmBtn, 1);
			returnLayout->addStretch();
        }

        returnWidget->setObjectName("requestItem");
        if (isConfirmed) {
            returnWidget->setStyleSheet("background-color: #E8F5E9;");
        }
        else {
            returnWidget->setStyleSheet("background-color: #FFFDE7;");
        }

        returnsLayout->addWidget(returnWidget);
    }

    returnsLayout->addStretch();
    
}

void LendingRequestsWidget::refreshRequests()
{
    refreshRequestsList();
}

void LendingRequestsWidget::refreshRequestsList()
{
    // UI-Elemente löschen
    QLayoutItem* item;
    while ((item = requestsLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    auto requests = db->getLendingRequests();

    if (requests.isEmpty()) {
        QLabel* noRequestsLabel = new QLabel("Keine Ausleihanfragen vorhanden.", this);
        noRequestsLabel->setAlignment(Qt::AlignCenter);
        requestsLayout->addWidget(noRequestsLabel);
        return;
    }

    // Kopfzeile erstellen
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 5, 10, 5);

    QLabel* bookIdHeader = new QLabel("ID", headerWidget);
    bookIdHeader->setFixedWidth(60);
    QLabel* titleHeader = new QLabel("Buchtitel", headerWidget);
    titleHeader->setFixedWidth(350);
    QLabel* authorHeader = new QLabel("Autor", headerWidget);
    authorHeader->setFixedWidth(200);
    QLabel* userHeader = new QLabel("Benutzer", headerWidget);
    userHeader->setFixedWidth(150);
    QLabel* dateHeader = new QLabel("Anfragedatum", headerWidget);
    dateHeader->setFixedWidth(200);
    QLabel* statusHeader = new QLabel("Status", headerWidget);
    statusHeader->setFixedWidth(200);
    QLabel* actionsHeader = new QLabel("Aktionen", headerWidget);

    headerLayout->addWidget(bookIdHeader);
    headerLayout->addWidget(titleHeader);
    headerLayout->addWidget(authorHeader);
    headerLayout->addWidget(userHeader);
    headerLayout->addWidget(dateHeader);
    headerLayout->addWidget(statusHeader);
    headerLayout->addWidget(actionsHeader, 1);

    headerWidget->setObjectName("requestsHeader");
    requestsLayout->addWidget(headerWidget);

    // Anfragen durchlaufen und anzeigen
    for (const auto& request : requests) {
        QWidget* requestWidget = new QWidget(this);
        QHBoxLayout* requestLayout = new QHBoxLayout(requestWidget);
        requestLayout->setContentsMargins(10, 5, 10, 5);

        QString bookId = request["book_id"].toString();
        QString title = request["title"].toString();
        QString author = request["author"].toString();
        QString username = request["username"].toString();
        QDateTime requestDate = request["request_date"].toDateTime();
        QString dateStr = requestDate.toString("dd.MM.yyyy hh:mm");
        QString status = request["status"].toString();
        int requestId = request["id"].toInt();

        QLabel* bookIdLabel = new QLabel(bookId, requestWidget);
        bookIdLabel->setFixedWidth(60);
        QLabel* titleLabel = new QLabel(title, requestWidget);
        titleLabel->setFixedWidth(350);
        QLabel* authorLabel = new QLabel(author, requestWidget);
        authorLabel->setFixedWidth(200);
        QLabel* userLabel = new QLabel(username, requestWidget);
        userLabel->setFixedWidth(150);
        QLabel* dateLabel = new QLabel(dateStr, requestWidget);
        dateLabel->setFixedWidth(200);
        QLabel* statusLabel = new QLabel(status, requestWidget);
        statusLabel->setFixedWidth(200);

        // Farbliche Kennzeichnung je nach Status
        if (status == "pending") {
            statusLabel->setStyleSheet("color: orange;");
            statusLabel->setText("Ausstehend");
        }
        else if (status == "approved") {
            statusLabel->setStyleSheet("color: green;");
            statusLabel->setText("Genehmigt");
        }
        else if (status == "rejected") {
            statusLabel->setStyleSheet("color: red;");
            statusLabel->setText("Abgelehnt");
        }

        requestLayout->addWidget(bookIdLabel);
        requestLayout->addWidget(titleLabel);
        requestLayout->addWidget(authorLabel);
        requestLayout->addWidget(userLabel);
        requestLayout->addWidget(dateLabel);
        requestLayout->addWidget(statusLabel);

        // Aktionsbuttons nur für ausstehende Anfragen anzeigen
        if (status == "pending") {
            QWidget* actionsWidget = new QWidget(requestWidget);
            QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
            actionsLayout->setContentsMargins(0, 0, 0, 0);

            QPushButton* approveBtn = new QPushButton("Genehmigen", actionsWidget);
            approveBtn->setObjectName("actionButton");
			approveBtn->setFixedHeight(35);
            approveBtn->setProperty("requestId", requestId);
			approveBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

            QPushButton* rejectBtn = new QPushButton("Ablehnen", actionsWidget);
            rejectBtn->setObjectName("cancelButton");
			rejectBtn->setFixedHeight(35);
            rejectBtn->setProperty("requestId", requestId);
			rejectBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

            connect(approveBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onApproveClicked);
            connect(rejectBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onRejectClicked);

            actionsLayout->addWidget(approveBtn);
            actionsLayout->addWidget(rejectBtn);
            actionsLayout->addStretch();

            requestLayout->addWidget(actionsWidget, 1);
        }
        else {
            QWidget* spacerWidget = new QWidget(requestWidget);
            requestLayout->addWidget(spacerWidget, 1);
        }

        requestWidget->setObjectName("requestItem");
        if (status == "pending") {
            requestWidget->setStyleSheet("background-color: #FFFDE7;");
        }
        else if (status == "approved") {
            requestWidget->setStyleSheet("background-color: #E8F5E9;");
        }
        else if (status == "rejected") {
            requestWidget->setStyleSheet("background-color: #FFEBEE;");
        }

        requestsLayout->addWidget(requestWidget);
    }

    requestsLayout->addStretch();
}

void LendingRequestsWidget::onApproveClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int requestId = btn->property("requestId").toInt();

    if (db->approveLendingRequest(requestId)) {
        QMessageBox::information(this, "Erfolg", "Ausleihanfrage wurde genehmigt!");
        refreshRequests();
    }
    else {
        QMessageBox::warning(this, "Fehler", "Konnte die Anfrage nicht bearbeiten.");
    }
}

void LendingRequestsWidget::onRejectClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int requestId = btn->property("requestId").toInt();

    if (db->rejectLendingRequest(requestId)) {
        QMessageBox::information(this, "Erfolg", "Ausleihanfrage wurde abgelehnt!");
        refreshRequests();
    }
    else {
        QMessageBox::warning(this, "Fehler", "Konnte die Anfrage nicht bearbeiten.");
    }
}


void LendingRequestsWidget::onConfirmReturnClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    int lendingId = button->property("lendingId").toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Rückgabe bestätigen",
        "Möchten Sie die Rückgabe bestätigen?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (db->confirmBookReturn(lendingId)) {
            QMessageBox::information(this, "Erfolg", "Die Rückgabe wurde erfolgreich bestätigt.");
            refreshReturnsList(); // <--- Ansicht aktualisieren!
        }
        else {
            QMessageBox::warning(this, "Fehler", "Die Rückgabe konnte nicht bestätigt werden.");
        }
    }
}
