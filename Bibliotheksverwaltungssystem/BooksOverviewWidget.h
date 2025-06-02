#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include "DatabaseManager.h"

class BooksOverviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BooksOverviewWidget(DatabaseManager* db, const QString& role, QWidget* parent = nullptr);
    void reloadBooks(const QString& filter = "");

signals:
    void editBook(int bookId);
    void deleteBook(int bookId);
    void lendBook(int bookId);
    void returnBook(int bookId);

private slots:
    void onSearchTextChanged(const QString& text);
    void onActionClicked();

private:
    QWidget* createBookCard(int bookId, const QString& title, const QString& author,
        int year, const QString& status, const QString& description,
        const QByteArray& imageData);

    DatabaseManager* db;
    QString role;
    QLineEdit* searchEdit;
    QLabel* headline;
    QWidget* booksContainer;
    QGridLayout* booksLayout;
};