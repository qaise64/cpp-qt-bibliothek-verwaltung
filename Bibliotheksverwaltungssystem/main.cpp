#include "Bibliotheksverwaltungssystem.h"
#include <QtWidgets/QApplication>
#include "DatabaseManager.h"
#include "LoginWindow.h"
#include "MainWindow.h"
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
    MainWindow mainWindow;


    QObject::connect(&loginWindow, &LoginWindow::loginRequested, [&](const QString& user, const QString& pass, const QString& role) {
        mainWindow.setRole(role); // setRole musst du in MainWindow implementieren
        mainWindow.show();
        loginWindow.close();
        });

    loginWindow.show();
    return a.exec();

}


