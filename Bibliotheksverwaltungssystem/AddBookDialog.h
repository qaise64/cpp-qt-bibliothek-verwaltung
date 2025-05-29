#pragma once

#include <QDialog>
#include <QByteArray>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class AddBookDialog : public QWidget
{
    Q_OBJECT

public:
    explicit AddBookDialog(QWidget* parent = nullptr);
    ~AddBookDialog();

    QString getTitle() const;
    QString getAuthor() const;
    int getYear() const;
    QString getStatus() const;
    QString getDescription() const;
    QByteArray getImageData() const;
    QPushButton* getOkButton() const { return okButton; }
    QPushButton* getCancelButton() const { return cancelButton; }

private slots:
    void on_selectImageButton_clicked();

signals:
    void accepted();
    void rejected();

private:
    QLineEdit* titleEdit;
    QLineEdit* authorEdit;
    QSpinBox* yearSpinBox;
    QComboBox* statusComboBox;
    QTextEdit* descriptionEdit;
    QLabel* imageLabel;
    QPushButton* selectImageButton;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QByteArray imageData;
};