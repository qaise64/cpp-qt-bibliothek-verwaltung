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
    setCentralWidget(central);
	central->setObjectName("centralWidget");

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	// Navigation startet hier


	QWidget* navBar = new QWidget(central);
    navBar->setFixedHeight(50);
	navBar->setObjectName("navBar");
    
	QHBoxLayout* navLayout = new QHBoxLayout(navBar);
    navBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	QLabel* logoLabel = new QLabel("<b>LibraryPro</b>");
    logoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	logoLabel->setObjectName("logoLabel");

	QWidget* CenterWidget = new QWidget();
	QHBoxLayout* centerLayout = new QHBoxLayout(CenterWidget);
	centerLayout->setContentsMargins(0, 0, 0, 0);
	centerLayout->setSpacing(5);


	QPushButton* userButton = new QPushButton("Benutzer");
	userButton->setObjectName("userButton");
	QPushButton* librarianButton = new QPushButton("Bibliothekar");
    librarianButton->setObjectName("librarianButton");

	centerLayout->addWidget(userButton);
	centerLayout->addWidget(librarianButton);
    CenterWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	QWidget* RightWidget = new QWidget();
	QHBoxLayout* rightLayout = new QHBoxLayout(RightWidget);
	rightLayout->setContentsMargins(0, 0, 0, 0);
	rightLayout->setSpacing(5);
    RightWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	RightWidget->setObjectName("rightWidget");
    RightWidget->setMinimumHeight(110);
    RightWidget->setFixedHeight(30);


	QPushButton* minimizeButton = new QPushButton("");
    minimizeButton->setIcon(QIcon(":/resources/icons/navbar/minimize2.svg"));
	minimizeButton->setObjectName("minimizeButton");
	minimizeButton->setFixedSize(50,30);
    minimizeButton->setIconSize(QSize(20, 20));

	minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// Close Button
	QPushButton* closeButton = new QPushButton("");
    closeButton->setIcon(QIcon(":/resources/icons/navbar/close2.svg"));
    closeButton->setObjectName("closeButton");
	closeButton->setFixedSize(50, 30);
    closeButton->setIconSize(QSize(20, 20));

	closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	rightLayout->addWidget(minimizeButton);
	rightLayout->addWidget(closeButton);

	mainLayout->addWidget(navBar);
    navLayout->addWidget(logoLabel);      // Left
    navLayout->addStretch();              // Spacer
    navLayout->addWidget(CenterWidget);   // Center
    navLayout->addStretch();              // Spacer
    navLayout->addWidget(RightWidget);    // Right


    mainLayout->addWidget(navBar, 0, Qt::AlignTop);




    // Navigation endet hier


    /*

    navLayout = new QHBoxLayout();
    closeButton = new QPushButton("✕");
    closeButton->setFixedSize(30, 30);
    closeButton->setFlat(true);
    navLayout->addWidget(closeButton, 0, Qt::AlignLeft);
    navLayout->addWidget(navBar, 1);
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

    */

    // Vollbild
    showFullScreen();

    // Verbindungen
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
 connect(minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);

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