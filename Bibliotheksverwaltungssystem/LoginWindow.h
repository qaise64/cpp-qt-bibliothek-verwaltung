#pragma once

#include <QMainWindow>
#include <QString>


class QTabBar;
class QLabel;
class QWidget;
class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout; 

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow(); 

    QString getUsername() const;
    QString getPassword() const;
    QString getRole() const;
    void markForDeletion(bool beingDeleted = true) { m_isBeingDeleted = beingDeleted; }

signals:
    void loginRequested(const QString& username, const QString& password, const QString& role);

private slots:
    void onLoginClicked();
 

private:

    QWidget* centralWidget; 
    QVBoxLayout* mainLayout;


    QWidget* navBar;
    QHBoxLayout* navLayout;
    QLabel* logoLabel;

    QWidget* centerNavWidget; 
    QHBoxLayout* centerNavLayout; 
    QPushButton* userButton;
    QPushButton* librarianButton;

    QWidget* rightNavWidget; 
    QHBoxLayout* rightNavLayout; 
    QPushButton* minimizeButton;
    QPushButton* closeButton;

    QHBoxLayout* contentLayout; 

    QWidget* leftContentWidget; 
    QVBoxLayout* leftContentLayout; 
    QLabel* imageLabel;
    QLabel* willkommenLabel;

    QWidget* loginFormWidget;
    QVBoxLayout* rightFormLayout;
    QLabel* loginLabel;
    QWidget* formContainer;
    QVBoxLayout* formLayout;
    QLabel* userLabel;
    QLineEdit* usernameEdit;
    QLabel* passLabel;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;


    QString currentRole = "Benutzer"; 
    bool m_isBeingDeleted = false;


protected:
    void closeEvent(QCloseEvent* event) override;


};