#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>  
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLabel>
#include "DatabaseManager.h"

class UserLendingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit UserLendingsWidget(DatabaseManager* db, QWidget* parent = nullptr);
    void refreshLendings();

private slots:
    void onReturnClicked();
    void onExtendClicked();
    void onCancelRequestClicked();

private:
    DatabaseManager* db;
    QTableWidget* lendingsTable;
    void setupUI();
}; 