#include "Bibliotheksverwaltungssystem.h"
#include <QtWidgets/QApplication>
#include "DatabaseManager.h"
#include "LoginWindow.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
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
    QObject::connect(&loginWindow, &LoginWindow::loginRequested, [&](const QString& username, const QString& password, const QString& role){
        // Hier später: Authentifizierung gegen die Datenbank
        if (username == "admin" && password == "admin" && role == "Bibliothekar") {
            loginWindow.close();
            Bibliotheksverwaltungssystem w;
            w.show();
            a.exec();
        } else if (username == "user" && password == "user" && role == "Benutzer") {
            loginWindow.close();
            Bibliotheksverwaltungssystem w;
            w.show();
            a.exec();
        } else {
            QMessageBox::warning(&loginWindow, "Login fehlgeschlagen", "Ungültige Zugangsdaten oder Rolle.");
        }
    });

    loginWindow.show();
    return a.exec();
}
