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
    // --- Hauptlayout: vertikal zentriert alles ---
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // Stretch oben
    outerLayout->addStretch();

    // --- Zentrierter Bereich für Titel, Linien, Formular, Buttons ---
    QWidget* centerWidget = new QWidget(this);
    QVBoxLayout* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);

    // --- Titel ---
    QLabel* dialogTitle = new QLabel("Buch hinzufügen", this);
    dialogTitle->setAlignment(Qt::AlignCenter);
    dialogTitle->setObjectName("sectionTitle");
    centerLayout->addWidget(dialogTitle, 0, Qt::AlignCenter);

    // --- Linie unter Titel ---
    centerLayout->addSpacing(10);
    QFrame* titleLine = new QFrame(this);
    titleLine->setFrameShape(QFrame::HLine);
    titleLine->setFrameShadow(QFrame::Sunken);
    centerLayout->addWidget(titleLine);
    centerLayout->addSpacing(24);

    // --- Formular-Objekte ---
    titleEdit = new QLineEdit(this);
    authorEdit = new QLineEdit(this);
    yearSpinBox = new QSpinBox(this);
    statusComboBox = new QComboBox(this);
    descriptionEdit = new QTextEdit(this);
    imageLabel = new QLabel(this);
    selectImageButton = new QPushButton("Bild auswählen", this);
    okButton = new QPushButton("Hinzufügen", this);
    cancelButton = new QPushButton("Abbrechen", this);

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

    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QFont labelFont("Arial", 10);

    const int fieldHeight = 40;
    const int formSpacing = 18;

    titleEdit->setPlaceholderText("Titel eingeben...");
    authorEdit->setPlaceholderText("Autor eingeben...");
    descriptionEdit->setPlaceholderText("Kurze Beschreibung des Buchs...");

    yearSpinBox->setRange(1000, 2100);
    yearSpinBox->setValue(2024);
    yearSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);

    statusComboBox->addItems({ "verfügbar", "ausgeliehen" });

    descriptionEdit->setMinimumHeight(120);
    yearSpinBox->setFixedHeight(fieldHeight);
    statusComboBox->setFixedHeight(fieldHeight);

    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("Bildvorschau erscheint hier nach Auswahl");
    imageLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    descriptionEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    yearSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    statusComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // --- Linker Bereich: Titel, Autor, Jahr, Status ---
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->setSpacing(formSpacing);

    QLabel* titleLabel = new QLabel("Titel:", this);
    titleLabel->setFont(labelFont);
    leftLayout->addWidget(titleLabel);
    leftLayout->addWidget(titleEdit);

    QLabel* authorLabel = new QLabel("Autor:", this);
    authorLabel->setFont(labelFont);
    leftLayout->addWidget(authorLabel);
    leftLayout->addWidget(authorEdit);

    QHBoxLayout* yearStatusLayout = new QHBoxLayout;
    QLabel* yearLabel = new QLabel("Jahr:", this);
    yearLabel->setFont(labelFont);
    QLabel* statusLabel = new QLabel("Status:", this);
    statusLabel->setFont(labelFont);

    QVBoxLayout* yearLayout = new QVBoxLayout;
    yearLayout->addWidget(yearLabel);
    yearLayout->addWidget(yearSpinBox);

    QVBoxLayout* statusLayout = new QVBoxLayout;
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(statusComboBox);

    yearStatusLayout->addLayout(yearLayout, 1);
    yearStatusLayout->addSpacing(8);
    yearStatusLayout->addLayout(statusLayout, 1);

    leftLayout->addLayout(yearStatusLayout);
    leftLayout->addStretch();

    // --- Mittlerer Bereich: Beschreibung ---
    QVBoxLayout* centerFormLayout = new QVBoxLayout;
    centerFormLayout->setSpacing(formSpacing);
    QLabel* descLabel = new QLabel("Beschreibung:", this);
    descLabel->setFont(labelFont);
    centerFormLayout->addWidget(descLabel);
    centerFormLayout->addWidget(descriptionEdit, 1);
    centerFormLayout->addStretch();

    // --- Rechter Bereich: Cover & Buttons ---
    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(formSpacing);
    QLabel* coverLabel = new QLabel("Buchcover", this);
    coverLabel->setFont(labelFont);
    rightLayout->addWidget(coverLabel);
    rightLayout->addWidget(selectImageButton);
    rightLayout->addWidget(imageLabel, 1);
    rightLayout->addStretch();

    // --- Vertikale Trennlinien ---
    QFrame* line1 = new QFrame(this);
    line1->setFrameShape(QFrame::VLine);
    line1->setFrameShadow(QFrame::Sunken);

    QFrame* line2 = new QFrame(this);
    line2->setFrameShape(QFrame::VLine);
    line2->setFrameShadow(QFrame::Sunken);

    // --- Formular-Hauptlayout ---
    QWidget* formContainer = new QWidget(this);
    formContainer->setMaximumWidth(1400);
    formContainer->setMinimumWidth(1200);
    formContainer->setMaximumHeight(500);
    QHBoxLayout* mainLayout = new QHBoxLayout(formContainer);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);
    mainLayout->addLayout(leftLayout, 3);
    mainLayout->addWidget(line1);
    mainLayout->addLayout(centerFormLayout, 3);
    mainLayout->addWidget(line2);
    mainLayout->addLayout(rightLayout, 2);

    centerLayout->addWidget(formContainer, 0, Qt::AlignCenter);

    // --- Linie zwischen Formular und Buttons ---
    centerLayout->addSpacing(16);
    QFrame* formButtonLine = new QFrame(this);
    formButtonLine->setFrameShape(QFrame::HLine);
    formButtonLine->setFrameShadow(QFrame::Sunken);
    centerLayout->addWidget(formButtonLine);
    centerLayout->addSpacing(10);

    // --- Buttons ---
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacing(12);
    buttonLayout->addWidget(okButton);
    buttonLayout->addStretch();
    centerLayout->addLayout(buttonLayout);
    centerLayout->addSpacing(8);

    // --- Alles vertikal zentrieren ---
    outerLayout->addWidget(centerWidget, 0, Qt::AlignCenter);
    outerLayout->addStretch();

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