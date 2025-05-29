#include "Bibliotheksverwaltungssystem.h"
#include <QtWidgets/QApplication>
#include "DatabaseManager.h"
#include "LoginWindow.h"
#include "MainWindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTimer>

MainWindow* g_mainWindow = nullptr;

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QFile styleFile("style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&styleFile);
        a.setStyleSheet(ts.readAll());
    }

    DatabaseManager db;
    db.openDatabase("library.db");
    db.createTables();

    LoginWindow loginWindow;
    MainWindow mainWindow; 
    g_mainWindow = &mainWindow;

    QObject::connect(&loginWindow, &LoginWindow::loginRequested, [&](const QString& user, const QString& pass, const QString& role) {
        mainWindow.setRole(role);
        mainWindow.showFullScreen();
        QTimer::singleShot(1000, &loginWindow, &QWidget::hide);
    });

    QObject::connect(&mainWindow, &MainWindow::logoutRequested, [&]() {
        loginWindow.showFullScreen();
        QTimer::singleShot(1000, &mainWindow, &QWidget::hide);
    });

    loginWindow.showFullScreen();
    int result = a.exec();
    return result;
}