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
    void refreshPendingReturns();

private slots:
    void onApproveClicked();
    void onRejectClicked();
    void onConfirmReturnClicked();

private:
    DatabaseManager* db;
    QTableWidget* requestsTable;
    QTableWidget* pendingReturnsTable;
    QTabWidget* tabWidget;
    QVBoxLayout* requestsLayout = nullptr;
    QVBoxLayout* returnsLayout = nullptr;
    void setupUI();
    void refreshRequestsList();
    void refreshReturnsList(); 
};
