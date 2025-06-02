#include "UserLendingsWidget.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>

UserLendingsWidget::UserLendingsWidget(DatabaseManager* db, QWidget* parent)
    : QWidget(parent), db(db)
{
    setupUI();
    refreshLendings();
}

void UserLendingsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QLabel* title = new QLabel("Meine Ausleihen", this);
    title->setObjectName("sectionTitle");
    QFont f = title->font();
    f.setPointSize(16);
    f.setBold(true);
    title->setFont(f);

    mainLayout->addWidget(title);

    lendingsTable = new QTableWidget(this);
    lendingsTable->setColumnCount(7);
    QStringList headers;
    headers << "ID" << "Buch" << "Autor" << "Ausleihdatum" << "Fälligkeitsdatum" << "Status" << "Aktionen";
    lendingsTable->setHorizontalHeaderLabels(headers);
    lendingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    lendingsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lendingsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    mainLayout->addWidget(lendingsTable, 1);
}

void UserLendingsWidget::refreshLendings()
{
    lendingsTable->setRowCount(0);

    auto lendings = db->getUserLendings();
    lendingsTable->setRowCount(lendings.size());

    for (int i = 0; i < lendings.size(); i++) {
        QTableWidgetItem* idItem;
        if (lendings[i]["type"].toString() == "lending") {
            idItem = new QTableWidgetItem(lendings[i]["id"].toString());
        }
        else {
            idItem = new QTableWidgetItem(lendings[i]["request_id"].toString());
        }

        QTableWidgetItem* titleItem = new QTableWidgetItem(lendings[i]["title"].toString());
        QTableWidgetItem* authorItem = new QTableWidgetItem(lendings[i]["author"].toString());
        QTableWidgetItem* lendDateItem = new QTableWidgetItem(lendings[i]["lend_date"].toString());

        // Due Date nur für tatsächliche Ausleihen, nicht für Anfragen
        QTableWidgetItem* dueDateItem;
        if (lendings[i]["type"].toString() == "lending") {
            dueDateItem = new QTableWidgetItem(lendings[i]["due_date"].toString());
        }
        else {
            dueDateItem = new QTableWidgetItem("-");
        }

        // Status basierend auf dem Typ und dem Status
        QTableWidgetItem* statusItem;
        if (lendings[i]["type"].toString() == "lending") {
            bool returned = lendings[i]["returned"].toBool();
            statusItem = new QTableWidgetItem(returned ? "Zurückgegeben" : "Ausgeliehen");
        }
        else {
            QString status = lendings[i]["status"].toString();
            // Status übersetzen
            if (status == "pending")
                status = "Ausstehend";
            else if (status == "approved")
                status = "Genehmigt";
            else if (status == "rejected")
                status = "Abgelehnt";
            else if (status == "returned")
                status = "Zurückgegeben";
            else if (status == "canceled")
                status = "Abgebrochen";

            statusItem = new QTableWidgetItem(status);
        }

        lendingsTable->setItem(i, 0, idItem);
        lendingsTable->setItem(i, 1, titleItem);
        lendingsTable->setItem(i, 2, authorItem);
        lendingsTable->setItem(i, 3, lendDateItem);
        lendingsTable->setItem(i, 4, dueDateItem);
        lendingsTable->setItem(i, 5, statusItem);

        // Aktionsbuttons
        QWidget* buttonWidget = new QWidget();
        QHBoxLayout* buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(5);

        // Nur aktuelle Ausleihen oder genehmigte Anfragen können zurückgegeben werden
        if ((lendings[i]["type"].toString() == "lending" && !lendings[i]["returned"].toBool()) ||
            (lendings[i]["type"].toString() == "request" && lendings[i]["status"].toString() == "approved")) {

            // Button zum Zurückgeben
            QPushButton* returnBtn = new QPushButton("Zurückgeben");
            int lendingId;
            if (lendings[i]["type"].toString() == "lending") {
                lendingId = lendings[i]["id"].toInt();
            }
            else {
                lendingId = lendings[i]["request_id"].toInt();
            }
            returnBtn->setProperty("lendingId", lendingId);
            returnBtn->setProperty("isRequest", lendings[i]["type"].toString() == "request");
            returnBtn->setFixedHeight(30);
            returnBtn->setMinimumWidth(120);
            connect(returnBtn, &QPushButton::clicked, this, &UserLendingsWidget::onReturnClicked);
            buttonLayout->addWidget(returnBtn);

            // Neuer Button für Verlängerung (nur für tatsächliche Ausleihen, nicht für Anfragen)
            if (lendings[i]["type"].toString() == "lending" && !lendings[i]["extended"].toBool()) {
                QPushButton* extendBtn = new QPushButton("Verlängern");
                extendBtn->setProperty("lendingId", lendingId);
                extendBtn->setFixedHeight(30);
                extendBtn->setMinimumWidth(120);
                extendBtn->setStyleSheet("background-color: #2196F3; color: white;");
                connect(extendBtn, &QPushButton::clicked, this, &UserLendingsWidget::onExtendClicked);
                buttonLayout->addWidget(extendBtn);
            }
        }
        else if (lendings[i]["type"].toString() == "request" && lendings[i]["status"].toString() == "pending") {
            // Bei ausstehenden Anfragen: Abbrechen-Button anzeigen
            QPushButton* cancelBtn = new QPushButton("Abbrechen");
            cancelBtn->setProperty("requestId", lendings[i]["request_id"].toInt());
            cancelBtn->setFixedHeight(30);
            cancelBtn->setMinimumWidth(120);
            cancelBtn->setStyleSheet("background-color: #F44336; color: white;");
            connect(cancelBtn, &QPushButton::clicked, this, &UserLendingsWidget::onCancelRequestClicked);
            buttonLayout->addWidget(cancelBtn);
        }
        else {
            // Status-Text anzeigen
            QString labelText;

            if (lendings[i]["type"].toString() == "lending") {
                bool returned = lendings[i]["returned"].toBool();
                bool confirmed = lendings[i]["confirmed_return"].toBool();

                if (returned && !confirmed) {
                    labelText = "Zurückgegeben (warte auf Bestätigung)";
                }
                else if (returned && confirmed) {
                    labelText = "Rückgabe bestätigt";
                }
                else if (lendings[i]["extended"].toBool()) {
                    labelText = "Verlängert bis " + lendings[i]["due_date"].toDateTime().toString("dd.MM.yyyy");
                }
            }
            else if (lendings[i]["type"].toString() == "request") {
                QString status = lendings[i]["status"].toString();
                if (status == "pending") {
                    labelText = "Warte auf Genehmigung";
                }
                else if (status == "rejected") {
                    labelText = "Anfrage abgelehnt";
                }
                else if (status == "returned") {
                    labelText = "Zurückgegeben";
                }
                else if (status == "canceled") {
                    labelText = "Anfrage abgebrochen";
                }
            }

            QLabel* statusLabel = new QLabel(labelText);
            buttonLayout->addWidget(statusLabel);
        }

        lendingsTable->setCellWidget(i, 6, buttonWidget);

    }
}

void UserLendingsWidget::onReturnClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int lendingId = btn->property("lendingId").toInt();
    bool isRequest = btn->property("isRequest").toBool();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Buch zurückgeben",
        "Möchten Sie dieses Buch wirklich zurückgeben?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        bool success = false;

        if (isRequest) {
            // Behandle Ausleihanfragen
            success = db->returnLendingRequest(lendingId);
        }
        else {
            // Behandle tatsächliche Ausleihen
            success = db->returnBook(lendingId);
        }

        if (success) {
            QMessageBox::information(this, "Erfolg", "Buch wurde erfolgreich zurückgegeben.");
            refreshLendings();
        }
        else {
            QMessageBox::warning(this, "Fehler", "Buch konnte nicht zurückgegeben werden.");
        }
    }
}

void UserLendingsWidget::onExtendClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int lendingId = btn->property("lendingId").toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Ausleihe verlängern",
        "Möchten Sie diese Ausleihe um weitere 10 Tage verlängern?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (db->extendLending(lendingId)) {
            QMessageBox::information(this, "Erfolg", "Ausleihe wurde erfolgreich verlängert.");
            refreshLendings();
        }
        else {
            QMessageBox::warning(this, "Fehler", "Ausleihe konnte nicht verlängert werden. Möglicherweise wurde sie bereits verlängert.");
        }
    }
}

void UserLendingsWidget::onCancelRequestClicked()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int requestId = btn->property("requestId").toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Anfrage abbrechen",
        "Möchten Sie diese Ausleihanfrage wirklich abbrechen?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (db->cancelLendingRequest(requestId)) {
            QMessageBox::information(this, "Erfolg", "Ausleihanfrage wurde erfolgreich abgebrochen.");
            refreshLendings();
        }
        else {
            QMessageBox::warning(this, "Fehler", "Ausleihanfrage konnte nicht abgebrochen werden.");
        }
    }
}
