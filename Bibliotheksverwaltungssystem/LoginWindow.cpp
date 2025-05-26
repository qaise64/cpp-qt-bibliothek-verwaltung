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
#include <QSvgRenderer>
#include <QPainter>

LoginWindow::LoginWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
	central->setObjectName("centralWidget");

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
	mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

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
	centerLayout->setSpacing(15);
	CenterWidget->setFixedWidth(350);


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

    navLayout->addWidget(logoLabel);      // Left
    navLayout->addStretch();              // Spacer
    navLayout->addWidget(CenterWidget);   // Center
    navLayout->addStretch();              // Spacer
    navLayout->addWidget(RightWidget);    // Right


    mainLayout->addWidget(navBar, 0, Qt::AlignTop);

    // Navigation endet hier

    // 50/50 Layout (Mitte)
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);


    // Linke Seite: Bild

	QWidget* leftWidget = new QWidget();
	leftWidget->setObjectName("leftWidget");
	leftWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	leftWidget->setMinimumWidth(400);

    QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);


    imageLabel = new QLabel(leftWidget);



    // Desired size to render the SVG
    int targetWidth = 1000;

    // Load SVG renderer
    QSvgRenderer renderer(QString(":/resources/icons/navbar/user.svg"));

    // Calculate target height to keep aspect ratio
    QSize originalSize = renderer.defaultSize();
    int targetHeight = (originalSize.height() * targetWidth) / originalSize.width();

    // Prepare a transparent pixmap of target size
    QPixmap pixmap(targetWidth, targetHeight);
    pixmap.fill(Qt::transparent);

    // Render SVG into pixmap
    QPainter painter(&pixmap);
    renderer.render(&painter);

    // Set pixmap on label
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
    hLayout->addWidget(leftWidget, 1);

    QLabel* willkommenLabel = new QLabel("<b>Willkommen bei LibraryPro</b>");
    willkommenLabel->setObjectName("willkommenLabel");
    willkommenLabel->setAlignment(Qt::AlignCenter);


    leftLayout->addStretch();
    leftLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);
    leftLayout->addWidget(willkommenLabel, 0, Qt::AlignHCenter);
    leftLayout->addStretch();


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