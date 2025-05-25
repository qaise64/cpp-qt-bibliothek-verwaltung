#pragma once

#include <QMainWindow>

class QTabBar;
class QLabel;
class QWidget;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QPushButton;

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    QString getUsername() const;
    QString getPassword() const;
    QString getRole() const;

signals:
    void loginRequested(const QString& username, const QString& password, const QString& role);

private slots:
    void onLoginClicked();

private:
    QTabBar* tabBar;
    QLabel* imageLabel;
    QWidget* loginFormWidget;
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;
    QPushButton* cancelButton;
    QHBoxLayout* navLayout;
    QPushButton* closeButton;
};