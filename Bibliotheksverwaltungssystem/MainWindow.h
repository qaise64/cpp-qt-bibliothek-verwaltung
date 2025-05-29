#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QVector>
#include "DatabaseManager.h" 

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    void setRole(const QString& role);
    void showCentralContent(QWidget* contentWidget, QSize fixedSize);

signals:
    void logoutRequested();

private slots:
    void addBookDialog();
    

private:
    QString currentRole;
    DatabaseManager db;

    QWidget* centralWidget;
    QVBoxLayout* mainLayout;

    // Navbar
    QWidget* navBar;
    QHBoxLayout* navLayout;
    QLabel* logoLabel;
    QWidget* centerNavWidget;
    QHBoxLayout* centerNavLayout;
    QPushButton* logoutButton;
    QLabel* statusLabel;
    QWidget* rightNavWidget;
    QHBoxLayout* rightNavLayout;
    QPushButton* minimizeButton;
    QPushButton* closeButton;

    // Zweite Navbar (Funktionsleiste)
    QWidget* functionBar = nullptr;
    QHBoxLayout* functionBarLayout = nullptr;
    QVector<QPushButton*> functionButtons;


    // Main Content Widget

    QWidget* mainContentWidget = nullptr;
    QVBoxLayout* mainContentLayout = nullptr;

    void setupNavbar();
    void setupFunctionBar(const QString& role);
    void clearFunctionBar();
};