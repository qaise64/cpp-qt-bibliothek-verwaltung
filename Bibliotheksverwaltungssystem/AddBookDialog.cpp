#include "AddBookDialog.h"
#include <QFileDialog>
#include <QPixmap>
#include <QBuffer>
#include <QImageReader>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QFrame>
#include <QFont>

AddBookDialog::AddBookDialog(QWidget* parent)
    : QWidget(parent)

{
	// Objekt-Initialisierung

	titleEdit = new QLineEdit(this);
	authorEdit = new QLineEdit(this);
	yearSpinBox = new QSpinBox(this);
	statusComboBox = new QComboBox(this);
	descriptionEdit = new QTextEdit(this);
	imageLabel = new QLabel(this);
	selectImageButton = new QPushButton("Bild auswählen", this);
	okButton = new QPushButton("Hinzufügen", this);
	cancelButton = new QPushButton("Abbrechen", this);

    // Objekt-Namen setzen

    this->setObjectName("addBookWidget");
    titleEdit->setObjectName("titleEdit");
    authorEdit->setObjectName("authorEdit");
    yearSpinBox->setObjectName("yearSpinBox");
    statusComboBox->setObjectName("statusComboBox");
    descriptionEdit->setObjectName("descriptionEdit");
    imageLabel->setObjectName("imageLabel");
    selectImageButton->setObjectName("selectImageButton");
    okButton->setObjectName("okButton");
    cancelButton->setObjectName("dialogCancelButton");

    // Feste Breite für das Formular

    this->setFixedWidth(600);
	this->setFixedHeight(1000);

    // FONTS KONFIGURIEREN

    QFont sectionFont("Arial", 12, QFont::Bold);
    QFont labelFont("Arial", 10);
    QFont fieldFont("Arial", 10);

    // ALLGEMEINE STYLING-PARAMETER

    const int fieldHeight = 40;
    const int formSpacing = 24;  
    const int labelSpacing = 8;  

    // Placeholder-Texte
    titleEdit->setPlaceholderText("Titel eingeben...");
    authorEdit->setPlaceholderText("Autor eingeben...");
    descriptionEdit->setPlaceholderText("Kurze Beschreibung des Buchs...");

    // Jahr-Spinbox konfigurieren
    yearSpinBox->setRange(1000, 2100);
    yearSpinBox->setValue(2024);
    yearSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);

    // Statusdropdown konfigurieren
    statusComboBox->addItems({ "verfügbar", "ausgeliehen" });

    // Einheitliche Höhe für alle Eingabefelder

    descriptionEdit->setMinimumHeight(120);
    
    // Jahr- und Status-Feldbreiten
    yearSpinBox->setFixedWidth(250);
    statusComboBox->setFixedWidth(250);

    // Bild-Vorschau konfigurieren
    imageLabel->setFixedSize(300, 200);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("Bildvorschau erscheint hier nach Auswahl");
    imageLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    // SECTION DIVIDERS (horizontale Trennlinien)
    auto createDivider = [this]() {
        QFrame* line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        return line;
        };

    // SECTION TITLES (Abschnittsüberschriften)
    auto createSectionTitle = [this, sectionFont](const QString& title) {
        QLabel* label = new QLabel(title, this);
        label->setFont(sectionFont);
        return label;
        };

    // LAYOUTS ERSTELLEN

    // Hauptlayout mit größerem Rand
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->setSpacing(formSpacing);

    // Abschnitt: Buchinformationen
    mainLayout->addWidget(createSectionTitle("Buchinformationen"));

    // Titel
    QLabel* titleLabel = new QLabel("Titel:", this);
    titleLabel->setFont(labelFont);
    QVBoxLayout* titleLayout = new QVBoxLayout;
    titleLayout->setSpacing(labelSpacing);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(titleEdit);
    mainLayout->addLayout(titleLayout);

    // Autor
    QLabel* authorLabel = new QLabel("Autor:", this);
    authorLabel->setFont(labelFont);
    QVBoxLayout* authorLayout = new QVBoxLayout;
    authorLayout->setSpacing(labelSpacing);
    authorLayout->addWidget(authorLabel);
    authorLayout->addWidget(authorEdit);
    mainLayout->addLayout(authorLayout);

    // Jahr und Status in einer Reihe mit eigenen Labels
    QLabel* yearLabel = new QLabel("Jahr:", this);
    yearLabel->setFont(labelFont);
    QLabel* statusLabel = new QLabel("Status:", this);
    statusLabel->setFont(labelFont);

    // Container für Jahr und Status
    QHBoxLayout* yearStatusContainer = new QHBoxLayout;
    yearStatusContainer->setSpacing(20);

    // Jahr-Container
    QVBoxLayout* yearLayout = new QVBoxLayout;
    yearLayout->setSpacing(labelSpacing);
    yearLayout->addWidget(yearLabel);
    yearLayout->addWidget(yearSpinBox);
    yearStatusContainer->addLayout(yearLayout);

    // Status-Container
    QVBoxLayout* statusLayout = new QVBoxLayout;
    statusLayout->setSpacing(labelSpacing);
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(statusComboBox);
    yearStatusContainer->addLayout(statusLayout);

    // Füge restlichen Platz als Dehnung hinzu
    yearStatusContainer->addStretch();

    mainLayout->addLayout(yearStatusContainer);

    // Beschreibung
    QLabel* descLabel = new QLabel("Beschreibung:", this);
    descLabel->setFont(labelFont);
    mainLayout->addWidget(descLabel);
    mainLayout->addWidget(descriptionEdit);

    // Trennlinie nach Buchinformationen
    mainLayout->addWidget(createDivider());

    // Abschnitt: Buchcover
    mainLayout->addWidget(createSectionTitle("Buchcover"));

    // Bildauswahl-Button und Vorschau

    selectImageButton->setFixedHeight(fieldHeight);
    selectImageButton->setCursor(Qt::PointingHandCursor);

    mainLayout->addWidget(selectImageButton);
    mainLayout->addWidget(imageLabel, 0, Qt::AlignCenter);

    // Trennlinie vor Aktionsbuttons
    mainLayout->addWidget(createDivider());
    mainLayout->addSpacing(10);

    // Aktion-Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();


    okButton->setFixedSize(150, fieldHeight);
    cancelButton->setFixedSize(150, fieldHeight);
    okButton->setCursor(Qt::PointingHandCursor);
    cancelButton->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacing(16);
    buttonLayout->addWidget(okButton);

    mainLayout->addLayout(buttonLayout);

    // Signal-Verbindungen
    connect(selectImageButton, &QPushButton::clicked, this, &AddBookDialog::on_selectImageButton_clicked);
    connect(okButton, &QPushButton::clicked, this, [this]() { emit accepted(); });
    connect(cancelButton, &QPushButton::clicked, this, [this]() { emit rejected(); });
}

AddBookDialog::~AddBookDialog() {}

QString AddBookDialog::getTitle() const { return titleEdit->text(); }
QString AddBookDialog::getAuthor() const { return authorEdit->text(); }
int AddBookDialog::getYear() const { return yearSpinBox->value(); }
QString AddBookDialog::getStatus() const { return statusComboBox->currentText(); }
QString AddBookDialog::getDescription() const { return descriptionEdit->toPlainText(); }
QByteArray AddBookDialog::getImageData() const { return imageData; }

void AddBookDialog::on_selectImageButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Bild auswählen", "", "Bilder (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            QBuffer buffer(&imageData);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, "JPG");
        }
    }
}


void AddBookDialog::setBookData(int id, const QString& title, const QString& author,
    int year, const QString& status, const QString& description,
    const QByteArray& imgData) {
    bookId = id;
    editMode = true;

    titleEdit->setText(title);
    authorEdit->setText(author);
    yearSpinBox->setValue(year);
    int index = statusComboBox->findText(status);
    if (index >= 0)
        statusComboBox->setCurrentIndex(index);
    descriptionEdit->setText(description);

    if (!imgData.isEmpty()) {
        imageData = imgData;
        QPixmap pixmap;
        pixmap.loadFromData(imageData);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Dialog-Titel anpassen
    QLabel* titleLabel = findChild<QLabel*>("titleLabel");
    if (titleLabel)
        titleLabel->setText("Buch bearbeiten");

    // Button-Text anpassen
    okButton->setText("Aktualisieren");
}