#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include "dbmanager.h"

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(DBManager *db, QWidget *parent = nullptr);
    int getUserId() const { return userId; }
    void resetToLogin();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onRegisterAccountClicked();
    void onBackClicked();
    void onLogoutClicked();

private:
    void setupLoginUI();
    void setupRegisterUI();
    void setupLoggedInUI();

    DBManager *dbManager;
    QVBoxLayout *mainLayout;

    QLineEdit *loginInput;
    QLineEdit *passwordInput;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QLabel *messageLabel;

    QLineEdit *emailInput;
    QLineEdit *confirmPasswordInput;
    QPushButton *registerAccountButton;
    QPushButton *backButton;

    QLabel *welcomeLabel;
    QPushButton *logoutButton;

    enum State { Login, Register, LoggedIn };
    State currentState = Login;

    int userId = -1;
};

#endif // LOGINWINDOW_H
