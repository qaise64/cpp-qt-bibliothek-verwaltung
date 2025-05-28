#include "MainWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("LibraryPro - Hauptfenster");

    tabWidget = new QTabWidget(this);

    // Bücher-Tab
    booksTab = new QWidget();
    QVBoxLayout* booksLayout = new QVBoxLayout(booksTab);
    booksLayout->addWidget(new QLabel("Bücherverwaltung (hier später Liste/Buttons)"));

    // Ausleihen-Tab
    loansTab = new QWidget();
    QVBoxLayout* loansLayout = new QVBoxLayout(loansTab);
    loansLayout->addWidget(new QLabel("Ausleihverwaltung (hier später Liste/Buttons)"));

    tabWidget->addTab(booksTab, "Bücher");
    tabWidget->addTab(loansTab, "Ausleihen");

    setCentralWidget(tabWidget);
}

void MainWindow::setRole(const QString& role)
{
    currentRole = role;

}