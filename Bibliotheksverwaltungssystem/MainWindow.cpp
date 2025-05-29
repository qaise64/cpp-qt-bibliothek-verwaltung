#include "MainWindow.h"
#include "AddBookDialog.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <DatabaseManager.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setObjectName("centralWidget");

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Navbar
    setupNavbar();
    mainLayout->addWidget(navBar, 0, Qt::AlignTop);

    // Funktionsleiste (zweite Navbar)
    setupFunctionBar("Benutzer"); // Default, wird durch setRole überschrieben
    mainLayout->addWidget(functionBar, 0, Qt::AlignTop);


	// Hauptinhalt

    mainContentWidget = new QWidget(centralWidget);
    mainContentWidget->setObjectName("mainContentWidget");
    mainContentLayout = new QVBoxLayout(mainContentWidget);
    mainContentWidget->setLayout(mainContentLayout);


    mainLayout->addWidget(mainContentWidget, 1); 


    // Vollbild
    showFullScreen();
}

void MainWindow::setupNavbar()
{
    navBar = new QWidget(centralWidget);
    navBar->setFixedHeight(50);
    navBar->setObjectName("navBar");

    navLayout = new QHBoxLayout(navBar);
    navBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    logoLabel = new QLabel("<b>LibraryPro</b>");
    logoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    logoLabel->setObjectName("logoLabel");

    // Center: Logout-Button + Status
    centerNavWidget = new QWidget();
    centerNavLayout = new QHBoxLayout(centerNavWidget);
    centerNavLayout->setContentsMargins(0, 0, 0, 0);
    centerNavLayout->setSpacing(15);
    centerNavWidget->setFixedWidth(350);

    logoutButton = new QPushButton("Logout");
    logoutButton->setObjectName("logoutButton");
    logoutButton->setFixedHeight(32);

    statusLabel = new QLabel("");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    centerNavLayout->addWidget(logoutButton);
    centerNavLayout->addWidget(statusLabel);
    centerNavWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Rechts: Minimize/Close
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
    minimizeButton->setFixedSize(50, 30);
    minimizeButton->setIconSize(QSize(20, 20));
    minimizeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

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

    // Verbindungen
    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        emit logoutRequested();
        });
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);

}

void MainWindow::clearFunctionBar()
{
    if (functionBarLayout) {
        QLayoutItem* item;
        while ((item = functionBarLayout->takeAt(0)) != nullptr) {
            if (QWidget* w = item->widget()) {
                w->deleteLater();
            }
            delete item;
        }
        functionButtons.clear();
    }
}

void MainWindow::setupFunctionBar(const QString& role)
{
    if (!functionBar) {
        functionBar = new QWidget(centralWidget);
        functionBarLayout = new QHBoxLayout(functionBar);
        functionBar->setFixedHeight(50);
        functionBar->setObjectName("functionBar");
        functionBarLayout->setContentsMargins(20, 0, 20, 0);
        functionBarLayout->setSpacing(18);
    }
    else {
        clearFunctionBar();
    }

    if (role == "Bibliothekar") {
        // Bibliothekar-Buttons
        struct ButtonInfo { QString text; QString iconPath; QString objName; };
        QVector<ButtonInfo> buttons = {
            { "Bücherübersicht", ":/resources/icons/funcbar/book-open-solid.svg", "btnBooksOverview" },
            { "Buch hinzufügen",  ":/resources/icons/funcbar/plus-solid.svg", "btnAddBook" },
            { "Ausleihen verwalten", ":/resources/icons/funcbar/chart-line-solid.svg", "btnManageLendings" },
            { "Statistiken", ":/resources/icons/funcbar/chart-simple-solid.svg", "btnStatistics" }

        };
        for (const auto& info : buttons) {
            QPushButton* btn = new QPushButton();
            btn->setText(info.text);
            btn->setObjectName(info.objName);
            btn->setIcon(QIcon(info.iconPath));
            btn->setIconSize(QSize(22, 22));
            btn->setFixedHeight(36);
            functionBarLayout->addWidget(btn);
            functionButtons.append(btn);

            if (info.objName == "btnAddBook") {
                connect(btn, &QPushButton::clicked, this, &MainWindow::addBookDialog);
            }
            // Weitere Buttons:
            //else if (info.objName == "btnBooksOverview") {
            //    connect(btn, &QPushButton::clicked, this, &MainWindow::showBooksOverview);
            //}

        }
    }
    else {
        // Benutzer-Buttons
        struct ButtonInfo { QString text; QString iconPath; QString objName; };
        QVector<ButtonInfo> buttons = {
            { "Bücherübersicht", ":/resources/icons/funcbar/book-open-solid.svg", "btnBooksOverview" },
            { "Meine Ausleihen", ":/resources/icons/funcbar/clipboard-solid.svg", "btnMyLendings" }
        };
        for (const auto& info : buttons) {
            QPushButton* btn = new QPushButton();
            btn->setText(info.text);
            btn->setObjectName(info.objName);
            btn->setIcon(QIcon(info.iconPath));
            btn->setIconSize(QSize(22, 22));
            btn->setFixedHeight(36);
            functionBarLayout->addWidget(btn);
            functionButtons.append(btn);
        }
    }
    functionBarLayout->addStretch();
}

void MainWindow::setRole(const QString& role)
{
    currentRole = role;
    if (statusLabel) {
        if (role == "Benutzer")
            statusLabel->setText("Angemeldet als Benutzer");
        else if (role == "Bibliothekar")
            statusLabel->setText("Angemeldet als Bibliothekar");
        else
            statusLabel->setText("");
    }
    setupFunctionBar(role);
  
}

void MainWindow::showCentralContent(QWidget* contentWidget, QSize fixedSize)
{
    // Vorherigen Inhalt entfernen
    QLayoutItem* item;
    while ((item = mainContentLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    // Zentrierender Container
    QWidget* centerContainer = new QWidget(mainContentWidget);
    QVBoxLayout* vLayout = new QVBoxLayout(centerContainer);
    vLayout->addStretch();

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addStretch();

    if (fixedSize.isValid())
        contentWidget->setFixedSize(fixedSize);

    hLayout->addWidget(contentWidget);
    hLayout->addStretch();

    vLayout->addLayout(hLayout);
    vLayout->addStretch();

    mainContentLayout->addWidget(centerContainer);
}

void MainWindow::addBookDialog()
{
    AddBookDialog* addBookWidget = new AddBookDialog(mainContentWidget);

    // Buttons umleiten: "Hinzufügen" und "Abbrechen"
    connect(addBookWidget->getOkButton(), &QPushButton::clicked, this, [=]() {
        db.addBook(
            addBookWidget->getTitle(),
            addBookWidget->getAuthor(),
            addBookWidget->getYear(),
            addBookWidget->getStatus(),
            addBookWidget->getDescription(),
            addBookWidget->getImageData()
        );
        showCentralContent(new QLabel("Buch erfolgreich hinzugefügt!", mainContentWidget), QSize(300, 100));
        });

    connect(addBookWidget->getCancelButton(), &QPushButton::clicked, this, [=]() {
        showCentralContent(new QLabel("Vorgang abgebrochen.", mainContentWidget), QSize(300, 100));
        });

    showCentralContent(addBookWidget, QSize(600, 1000));
}