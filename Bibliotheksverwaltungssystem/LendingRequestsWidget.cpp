#include "LendingRequestsWidget.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

LendingRequestsWidget::LendingRequestsWidget(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), db(db)
{
    setupUI();
    refreshRequests();


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
    requestsTabLayout->setSpacing(10);
    requestsWidget->setObjectName("requestsContainer");

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

    // Modernes Layout für Anfragen
    QWidget* requestsContainer = new QWidget(requestsWidget);
    requestsLayout = new QVBoxLayout(requestsContainer);
    requestsLayout->setContentsMargins(0, 0, 0, 0);
    requestsLayout->setSpacing(10);
    requestsContainer->setObjectName("requestsContainer");

    requestsTabLayout->addWidget(requestsContainer);

    // Tab 2: Ausstehende Rückgaben
    QWidget* returnsWidget = new QWidget(tabWidget);
    QVBoxLayout* returnsLayout = new QVBoxLayout(returnsWidget);
    returnsLayout->setContentsMargins(0, 0, 0, 0);
    returnsLayout->setSpacing(10);
    returnsWidget->setObjectName("returnsContainer");

    // Tabelle für ausstehende Rückgaben
    pendingReturnsTable = new QTableWidget(returnsWidget);
    pendingReturnsTable->setColumnCount(6);
    QStringList returnHeaders;
    returnHeaders << "ID" << "Benutzer" << "Buch" << "Ausgeliehen am" << "Zurückgegeben am" << "Aktionen";
    pendingReturnsTable->setHorizontalHeaderLabels(returnHeaders);
    pendingReturnsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pendingReturnsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    pendingReturnsTable->setColumnWidth(0, 60);
    pendingReturnsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pendingReturnsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pendingReturnsTable->verticalHeader()->setDefaultSectionSize(44);
    pendingReturnsTable->verticalHeader()->setVisible(false);
    pendingReturnsTable->setAlternatingRowColors(true);
    returnsLayout->addWidget(pendingReturnsTable);

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
            refreshPendingReturns();
        }
        });
}

void LendingRequestsWidget::refreshRequests()
{
    // Aktuelle Implementierung beibehalten
    requestsTable->setRowCount(0);

    auto requests = db->getLendingRequests();
    requestsTable->setRowCount(requests.size());

    for (int i = 0; i < requests.size(); i++) {
        QTableWidgetItem* idItem = new QTableWidgetItem(requests[i]["id"].toString());
        QTableWidgetItem* userItem = new QTableWidgetItem(requests[i]["username"].toString());
        QTableWidgetItem* titleItem = new QTableWidgetItem(requests[i]["title"].toString());
        QTableWidgetItem* authorItem = new QTableWidgetItem(requests[i]["author"].toString());
        QTableWidgetItem* dateItem = new QTableWidgetItem(requests[i]["request_date"].toString());

        requestsTable->setItem(i, 0, idItem);
        requestsTable->setItem(i, 1, userItem);
        requestsTable->setItem(i, 2, titleItem);
        requestsTable->setItem(i, 3, authorItem);
        requestsTable->setItem(i, 4, dateItem);


        // Action-Buttons
        QWidget* buttonWidget = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

        // Status aus den Anfragedaten holen
        QString status = requests[i]["status"].toString();

        if (status == "pending") {
            QPushButton* approveBtn = new QPushButton("Genehmigen");
            approveBtn->setObjectName("actionButton");
            approveBtn->setProperty("requestId", requests[i]["id"].toInt());
            connect(approveBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onApproveClicked);

            QPushButton* rejectBtn = new QPushButton("Ablehnen");
            rejectBtn->setObjectName("lendingcancelButton");
            rejectBtn->setProperty("requestId", requests[i]["id"].toInt());
            rejectBtn->setFixedHeight(32);
            rejectBtn->setMinimumWidth(90);
            connect(rejectBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onRejectClicked);

            buttonLayout->addWidget(approveBtn);
            buttonLayout->addWidget(rejectBtn);
        }

        else {
            QLabel* statusLabel = new QLabel(status == "approved" ? "Genehmigt" : "Abgelehnt");
            buttonLayout->addWidget(statusLabel);
        }

        requestsTable->setCellWidget(i, 5, buttonWidget);
        requestsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        requestsTable->setColumnWidth(0, 60);

    }

    // Auch das neue Layout aktualisieren
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

    QLabel* titleHeader = new QLabel("Buchtitel", headerWidget);
    QLabel* userHeader = new QLabel("Benutzer", headerWidget);
    QLabel* dateHeader = new QLabel("Anfragedatum", headerWidget);
    QLabel* statusHeader = new QLabel("Status", headerWidget);
    QLabel* actionsHeader = new QLabel("Aktionen", headerWidget);

    titleHeader->setFixedWidth(200);
    userHeader->setFixedWidth(150);
    dateHeader->setFixedWidth(150);
    statusHeader->setFixedWidth(100);

    headerLayout->addWidget(titleHeader);
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

        QString title = request["title"].toString();
        QString author = request["author"].toString();
        QString username = request["username"].toString();
        QDateTime requestDate = request["request_date"].toDateTime();
        QString dateStr = requestDate.toString("dd.MM.yyyy hh:mm");
        QString status = request["status"].toString();
        int requestId = request["id"].toInt();

        QLabel* titleLabel = new QLabel(title + " (" + author + ")", requestWidget);
        QLabel* userLabel = new QLabel(username, requestWidget);
        QLabel* dateLabel = new QLabel(dateStr, requestWidget);
        QLabel* statusLabel = new QLabel(status, requestWidget);

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

        titleLabel->setFixedWidth(200);
        userLabel->setFixedWidth(150);
        dateLabel->setFixedWidth(150);
        statusLabel->setFixedWidth(100);

        requestLayout->addWidget(titleLabel);
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
            approveBtn->setProperty("requestId", requestId);
            approveBtn->setFixedHeight(32);
            approveBtn->setMinimumWidth(90);

            QPushButton* rejectBtn = new QPushButton("Ablehnen", actionsWidget);
            rejectBtn->setObjectName("cancelButton");
            rejectBtn->setProperty("requestId", requestId);
            rejectBtn->setFixedHeight(32);
            rejectBtn->setMinimumWidth(90);

            connect(approveBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onApproveClicked);
            connect(rejectBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onRejectClicked);

            actionsLayout->addWidget(approveBtn);
            actionsLayout->addWidget(rejectBtn);
            actionsLayout->addStretch();

            requestLayout->addWidget(actionsWidget, 1);
        }

        else {
            // Dummy-Widget für die korrekte Ausrichtung
            QWidget* spacerWidget = new QWidget(requestWidget);
            requestLayout->addWidget(spacerWidget, 1);
        }

        requestWidget->setObjectName("requestItem");
        if (status == "pending") {
            requestWidget->setStyleSheet("background-color: #FFFDE7;"); // Helles Gelb für ausstehend
        }
        else if (status == "approved") {
            requestWidget->setStyleSheet("background-color: #E8F5E9;"); // Helles Grün für genehmigt
        }
        else if (status == "rejected") {
            requestWidget->setStyleSheet("background-color: #FFEBEE;"); // Helles Rot für abgelehnt
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

// Implementierung der fehlenden Methode onConfirmReturnClicked
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
            refreshPendingReturns();
        }
        else {
            QMessageBox::warning(this, "Fehler", "Die Rückgabe konnte nicht bestätigt werden.");
        }
    }
}

void LendingRequestsWidget::refreshPendingReturns()
{
    pendingReturnsTable->setRowCount(0);
    auto pendingReturns = db->getPendingReturns();

    pendingReturnsTable->setRowCount(pendingReturns.size());
    pendingReturnsTable->verticalHeader()->setDefaultSectionSize(44);
    pendingReturnsTable->verticalHeader()->setVisible(false);
    pendingReturnsTable->setAlternatingRowColors(true);

    for (int i = 0; i < pendingReturns.size(); i++) {
        QTableWidgetItem* idItem = new QTableWidgetItem(pendingReturns[i]["id"].toString());
        QTableWidgetItem* userItem = new QTableWidgetItem(pendingReturns[i]["username"].toString());
        QTableWidgetItem* titleItem = new QTableWidgetItem(pendingReturns[i]["title"].toString());

        QDateTime lendDate = pendingReturns[i]["lend_date"].toDateTime();
        QTableWidgetItem* lendDateItem = new QTableWidgetItem(lendDate.toString("dd.MM.yyyy hh:mm"));

        QDateTime returnDate = pendingReturns[i]["return_date"].toDateTime();
        QTableWidgetItem* returnDateItem = new QTableWidgetItem(returnDate.toString("dd.MM.yyyy hh:mm"));

        pendingReturnsTable->setItem(i, 0, idItem);
        pendingReturnsTable->setItem(i, 1, userItem);
        pendingReturnsTable->setItem(i, 2, titleItem);
        pendingReturnsTable->setItem(i, 3, lendDateItem);
        pendingReturnsTable->setItem(i, 4, returnDateItem);
        pendingReturnsTable->setRowHeight(i, 44);

        // Action-Button oder Status
        QWidget* buttonWidget = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 0, 0, 0);

        bool isConfirmed = pendingReturns[i]["confirmed_return"].toBool();

        if (isConfirmed) {
            QLabel* statusLabel = new QLabel("Bestätigt");
            statusLabel->setObjectName("lendingstatusLabel");
            buttonLayout->addWidget(statusLabel);
        }

        else {
            QPushButton* confirmBtn = new QPushButton("Bestätigen");
            confirmBtn->setObjectName("actionButton");
            confirmBtn->setProperty("lendingId", pendingReturns[i]["id"].toInt());
            confirmBtn->setFixedHeight(32);
            confirmBtn->setMinimumWidth(90);
            connect(confirmBtn, &QPushButton::clicked, this, &LendingRequestsWidget::onConfirmReturnClicked);
            buttonLayout->addWidget(confirmBtn);
        }

        buttonLayout->addStretch();
        pendingReturnsTable->setCellWidget(i, 5, buttonWidget);
        pendingReturnsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        pendingReturnsTable->setColumnWidth(0, 60);
    }
}