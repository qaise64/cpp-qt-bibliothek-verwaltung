
#include "LoginDialog.h"
#include "ui_LoginDialog.h"  // automatisch generierte Header durch uic
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)  // UI-Objekt erstellen
{
    ui->setupUi(this);  // UI einrichten

    // Buttons verbinden
    connect(ui->buttonLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &LoginDialog::reject);
}

LoginDialog::~LoginDialog()
{
    delete ui;  // UI-Objekt löschen
}

QString LoginDialog::getUsername() const
{
    return ui->lineEditUsername->text();
}

QString LoginDialog::getPassword() const
{
    return ui->lineEditPassword->text();
}

void LoginDialog::onLoginClicked()
{
    QString username = getUsername();
    QString password = getPassword();

    // Beispielhafte Validierung
    if (username == "admin" && password == "admin") {
        accept();
    }
    else {
        QMessageBox::warning(this, tr("Login fehlgeschlagen"), tr("Ungültiger Benutzername oder Passwort."));
    }
}

