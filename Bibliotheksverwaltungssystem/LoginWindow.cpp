#include "LoginWindow.h"
#include <QTabBar>
#include <QLabel>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPixmap>

LoginWindow::LoginWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // Navigation (oben)
    tabBar = new QTabBar();
    tabBar->addTab("Benutzer");
    tabBar->addTab("Bibliothekar");

    navLayout = new QHBoxLayout();
    closeButton = new QPushButton("✕");
    closeButton->setFixedSize(30, 30);
    closeButton->setFlat(true);
    navLayout->addWidget(closeButton, 0, Qt::AlignLeft);
    navLayout->addWidget(tabBar, 1);
    mainLayout->addLayout(navLayout);

    // 50/50 Layout (Mitte)
    QHBoxLayout* hLayout = new QHBoxLayout();

    // Linke Seite: Bild
    imageLabel = new QLabel();
    imageLabel->setPixmap(QPixmap(":/img/login_photo.jpg").scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setAlignment(Qt::AlignCenter);
    hLayout->addWidget(imageLabel, 1);

    // Rechte Seite: Login-Formular
    loginFormWidget = new QWidget();
    QVBoxLayout* formLayout = new QVBoxLayout(loginFormWidget);

    QLabel* userLabel = new QLabel("Benutzername:");
    usernameEdit = new QLineEdit();
    QLabel* passLabel = new QLabel("Passwort:");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Anmelden");

    formLayout->addWidget(userLabel);
    formLayout->addWidget(usernameEdit);
    formLayout->addWidget(passLabel);
    formLayout->addWidget(passwordEdit);
    formLayout->addSpacing(20);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(loginButton);
    formLayout->addLayout(buttonLayout);

    formLayout->addStretch(1);

    hLayout->addWidget(loginFormWidget, 1);

    mainLayout->addLayout(hLayout);
    setCentralWidget(central);

    // Vollbild
    showFullScreen();

    // Verbindungen
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
}

QString LoginWindow::getUsername() const
{
    return usernameEdit->text();
}

QString LoginWindow::getPassword() const
{
    return passwordEdit->text();
}

QString LoginWindow::getRole() const
{
    return tabBar->currentIndex() == 0 ? "Benutzer" : "Bibliothekar";
}

void LoginWindow::onLoginClicked()
{
    emit loginRequested(getUsername(), getPassword(), getRole());
}