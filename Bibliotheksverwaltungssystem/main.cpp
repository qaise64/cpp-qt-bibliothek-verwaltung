#include "Bibliotheksverwaltungssystem.h"
#include <QtWidgets/QApplication>
#include "DatabaseManager.h"
#include "LoginWindow.h"
#include "MainWindow.h"
#include <QFile>
#include <QTextStream>
#include <functional>

MainWindow* g_mainWindow = nullptr;

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    QFile styleFile("style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&styleFile);
        a.setStyleSheet(ts.readAll());
    }

    DatabaseManager db;
    db.openDatabase("library.db");
    db.createTables();

    LoginWindow* loginWindow = nullptr;
    std::function<void(const QString&, const QString&, const QString&)> handleLogin;

    // Neue Funktion zum Auftauchen des Login-Fensters
    auto showLogin = [&]() {
        if (!loginWindow) {
            loginWindow = new LoginWindow();
            QObject::connect(loginWindow, &LoginWindow::loginRequested, handleLogin);
        }
        loginWindow->showFullScreen();
        };

    handleLogin = [&](const QString& user, const QString& pass, const QString& role) {
        MainWindow* mainWindow = new MainWindow(&db);
        g_mainWindow = mainWindow;
        mainWindow->setRole(role);
        mainWindow->showFullScreen();
        loginWindow->hide();

        QObject::connect(mainWindow, &MainWindow::logoutRequested, [&, mainWindow]() {
            mainWindow->close();
            g_mainWindow = nullptr;

        
            loginWindow->markForDeletion(false);

            // Direkt anzeigen, ohne Zerstörung und Neuerstellung
            loginWindow->showFullScreen();
            });

        QObject::connect(mainWindow, &MainWindow::destroyed, [&]() {
            g_mainWindow = nullptr;
            });
        };

    showLogin();
    return a.exec();

}
