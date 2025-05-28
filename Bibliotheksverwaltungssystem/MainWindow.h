#pragma once

#include <QMainWindow>
#include <QTabWidget>
#include <QWidget>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    
    void setRole(const QString& role);

private:
    QString currentRole;

    QTabWidget* tabWidget;
    QWidget* booksTab;
    QWidget* loansTab;

    
};