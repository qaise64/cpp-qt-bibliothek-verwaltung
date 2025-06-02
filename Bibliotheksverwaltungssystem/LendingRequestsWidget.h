#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QHeaderView> 
#include <QMessageBox> 
#include "DatabaseManager.h"
#include <QTime>
#include <QDateTime>

class LendingRequestsWidget : public QWidget {
    Q_OBJECT

public:
    explicit LendingRequestsWidget(DatabaseManager* db, QWidget* parent = nullptr);
    void refreshRequests();
    void refreshPendingReturns(); // Neue Methode für ausstehende Rückgaben

private slots:
    void onApproveClicked();
    void onRejectClicked();
    void onConfirmReturnClicked(); // Neuer Slot für Rückgabebestätigung

private:
    DatabaseManager* db;
    QTableWidget* requestsTable;
    QTableWidget* pendingReturnsTable; // Neue Tabelle für ausstehende Rückgaben
    QVBoxLayout* requestsLayout;
    QTabWidget* tabWidget; // Tabs für Anfragen und Rückgaben
    void setupUI();
    void refreshRequestsList();
};

