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
#include <QStyle>
#include <QCloseEvent>
#include <QApplication>
#include "MainWindow.h"
#include <QTimer>

extern MainWindow* g_mainWindow;

void LoginWindow::closeEvent(QCloseEvent* event)
{
   
    if (!g_mainWindow || !g_mainWindow->isVisible()) {
       
        this->blockSignals(true);
        QApplication::exit(0);
    }
    else {
        QMainWindow::closeEvent(event);
    }


    event->accept();
}

LoginWindow::LoginWindow(QWidget* parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setObjectName("centralWidget");

    mainLayout = new QVBoxLayout(centralWidget);
	mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

	// Navigation startet hier


	navBar = new QWidget(centralWidget);
    navBar->setFixedHeight(50);
	navBar->setObjectName("navBar");
    
	navLayout = new QHBoxLayout(navBar);
    navBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	logoLabel = new QLabel("<b>LibraryPro</b>");
    logoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	logoLabel->setObjectName("logoLabel");

	centerNavWidget = new QWidget();
	centerNavLayout = new QHBoxLayout(centerNavWidget);
    centerNavLayout->setContentsMargins(0, 0, 0, 0);
    centerNavLayout->setSpacing(15);
    centerNavWidget->setFixedWidth(350);


	userButton = new QPushButton("Benutzer");
	userButton->setObjectName("userButton");
	userButton->setEnabled(false); // Benutzer-Button ist standardmäßig deaktiviert
	userButton->setFixedHeight(32);

	librarianButton = new QPushButton("Bibliothekar");
    librarianButton->setObjectName("librarianButton");
	librarianButton->setEnabled(true); // Bibliothekar-Button ist standardmäßig aktiviert
	librarianButton->setFixedHeight(32);

    centerNavLayout->addWidget(userButton);
    centerNavLayout->addWidget(librarianButton);
    centerNavWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	rightNavWidget = new QWidget();
	rightNavLayout = new QHBoxLayout(rightNavWidget);
    rightNavLayout->setContentsMargins(0, 0, 0, 0);
    rightNavLayout->setSpacing(5);
    rightNavWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    rightNavWidget->setObjectName("rightWidget");
    rightNavWidget->setMinimumHeight(110);
    rightNavWidget->setFixedHeight(30);


	minimizeButton = new QPushButton("");
    minimizeButton->setIcon(QIcon(":/resources/icons/navbar/minimize2.svg"));
	minimizeButton->setObjectName("minimizeButton");
	minimizeButton->setFixedSize(50,30);
    minimizeButton->setIconSize(QSize(20, 20));

	minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	// Close Button
	
    closeButton = new QPushButton("");
    closeButton->setIcon(QIcon(":/resources/icons/navbar/close2.svg"));
    closeButton->setObjectName("closeButton");
	closeButton->setFixedSize(50, 30);
    closeButton->setIconSize(QSize(20, 20));

	closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    rightNavLayout->addWidget(minimizeButton);
    rightNavLayout->addWidget(closeButton);

    navLayout->addWidget(logoLabel);      // Left
    navLayout->addStretch();              // Spacer
    navLayout->addWidget(centerNavWidget);   // Center
    navLayout->addStretch();              // Spacer
    navLayout->addWidget(rightNavWidget);    // Right


    mainLayout->addWidget(navBar, 0, Qt::AlignTop);

    // Navigation endet hier

    // 50/50 Layout (Mitte)
    contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);


    // Linke Seite: Bild

	leftContentWidget = new QWidget();
	leftContentWidget->setObjectName("leftWidget");
	leftContentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	leftContentWidget->setMinimumWidth(400);

    leftContentLayout = new QVBoxLayout(leftContentWidget);


    imageLabel = new QLabel(leftContentWidget);
    imageLabel->setAlignment(Qt::AlignCenter);


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

    auto updateLeftPanelAppearance = [&](const QString& role) {

        QString svgPath;
        QString objectNameForStyle;
        QString willkommenObjectName;

        if (role == "Benutzer") {
            svgPath = ":/resources/icons/navbar/user.svg";
            objectNameForStyle = "leftWidgetUser";
            willkommenObjectName = "willkommenLabel";
        }
        else {
            svgPath = ":/resources/icons/navbar/librarian.svg";
            objectNameForStyle = "leftWidgetLibrarian";
            willkommenObjectName = "willkommenLabelLibrarian"; 
        }

        QSvgRenderer renderer(svgPath);
        QSize originalSize = renderer.defaultSize();
        int targetWidth = 1000;
        int targetHeight = (originalSize.height() * targetWidth) / originalSize.width();
        QPixmap pixmap(targetWidth, targetHeight);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        renderer.render(&painter);
        imageLabel->setPixmap(pixmap);

        leftContentWidget->setObjectName(objectNameForStyle);
        leftContentWidget->style()->unpolish(leftContentWidget);
        leftContentWidget->style()->polish(leftContentWidget);

        willkommenLabel->setObjectName(willkommenObjectName);
        willkommenLabel->style()->unpolish(willkommenLabel);
        willkommenLabel->style()->polish(willkommenLabel);

    };

    contentLayout->addWidget(leftContentWidget, 1);

    willkommenLabel = new QLabel("<b>Willkommen bei LibraryPro</b>");
    willkommenLabel->setObjectName("willkommenLabel");
    willkommenLabel->setAlignment(Qt::AlignCenter);


    leftContentLayout->addStretch();
    leftContentLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);
    leftContentLayout->addWidget(willkommenLabel, 0, Qt::AlignHCenter);
    leftContentLayout->addStretch();


    // Rechte Seite: Login-Formular
    loginFormWidget = new QWidget();
    loginFormWidget->setObjectName("loginFormWidget");
    rightFormLayout = new QVBoxLayout(loginFormWidget);
    rightFormLayout->setContentsMargins(0, 0, 0, 0);

    // Login-Label oben
    loginLabel = new QLabel("Login");
    loginLabel->setObjectName("loginLabel");
    loginLabel->setAlignment(Qt::AlignHCenter);
    rightFormLayout->addStretch(2);
    rightFormLayout->addWidget(loginLabel, 0, Qt::AlignHCenter);


    // Formular-Container
    formContainer = new QWidget();
    formContainer->setObjectName("formContainer");
    formContainer->setMinimumWidth(350);
    formContainer->setMaximumWidth(450);
    formLayout = new QVBoxLayout(formContainer);
    formLayout->setAlignment(Qt::AlignHCenter);

    userLabel = new QLabel("Benutzername:");
	userLabel->setObjectName("userLabel");
    usernameEdit = new QLineEdit();
    passLabel = new QLabel("Passwort:");
	passLabel->setObjectName("passLabel");
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Anmelden");
	loginButton->setObjectName("loginButton");
	loginButton->setFixedHeight(45);

    formLayout->addWidget(userLabel);
    formLayout->addWidget(usernameEdit);
    formLayout->addWidget(passLabel);
    formLayout->addWidget(passwordEdit);
    formLayout->addSpacing(20);
    formLayout->addWidget(loginButton);

    rightFormLayout->addWidget(formContainer, 0, Qt::AlignHCenter);
    rightFormLayout->addStretch(2); // Für vertikale Zentrierung

    contentLayout->addWidget(loginFormWidget, 1);



    mainLayout->addLayout(contentLayout);
    setCentralWidget(centralWidget);


    // Vollbild
    showFullScreen();

    // Verbindungen
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(closeButton, &QPushButton::clicked, this, [=]() {
        QApplication::exit(0);
        });
    connect(minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);



    connect(userButton, &QPushButton::clicked, this, [=]() {
        currentRole = "Benutzer";
        userButton->setEnabled(false);
        librarianButton->setEnabled(true);
        updateLeftPanelAppearance(currentRole);
        });

    connect(librarianButton, &QPushButton::clicked, this, [=]() {
        currentRole = "Bibliothekar";
        userButton->setEnabled(true);
        librarianButton->setEnabled(false);
        updateLeftPanelAppearance(currentRole);
        });

}

QString LoginWindow::getRole() const
{
    return currentRole;
}


QString LoginWindow::getUsername() const
{
    return usernameEdit->text();
}

QString LoginWindow::getPassword() const
{
    return passwordEdit->text();
}


void LoginWindow::onLoginClicked()
{
    QString username = getUsername();
    QString password = getPassword();
    QString role = getRole();

    if (role == "Benutzer") {
        if (username == "user" && password == "user") {
            emit loginRequested(username, password, role);
        }
        else {
            QMessageBox::warning(this, "Login fehlgeschlagen", "Benutzername oder Passwort ist falsch.");
        }
    }
    else if (role == "Bibliothekar") {
        if (username == "admin" && password == "admin") {
            emit loginRequested(username, password, role);
        }
        else {
            QMessageBox::warning(this, "Login fehlgeschlagen", "Benutzername oder Passwort ist falsch.");
        }
    }
}

LoginWindow::~LoginWindow() {}
