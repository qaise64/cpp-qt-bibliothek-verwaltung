
#pragma once

#include <QDialog>
#include <QString>

namespace Ui {
    class LoginDialog;  // Vorwärts-Deklaration der von uic generierten Klasse
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

    QString getUsername() const;
    QString getPassword() const;

private:
    Ui::LoginDialog* ui;  // Zeiger auf die UI-Klasse, die von uic generiert wird

private slots:
    void onLoginClicked();
};

