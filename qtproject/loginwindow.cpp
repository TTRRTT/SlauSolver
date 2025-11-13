#include "loginwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QCryptographicHash>

LoginWindow::LoginWindow(DBManager *db, QWidget *parent)
    : QDialog(parent), dbManager(db)
{
    setWindowTitle("Аккаунт");
    resize(350, 250);

    mainLayout = new QVBoxLayout(this);

    setupLoginUI();
}

void LoginWindow::setupLoginUI()
{
    currentState = Login;
    userId = -1;

    qDeleteAll(this->findChildren<QWidget*>());

    loginInput = new QLineEdit(this);
    loginInput->setPlaceholderText("Логин");

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Пароль");
    passwordInput->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Войти", this);
    registerButton = new QPushButton("Зарегистрировать новый аккаунт", this);

    messageLabel = new QLabel(this);
    messageLabel->setStyleSheet("color: red;");

    mainLayout->addWidget(new QLabel("Логин:", this));
    mainLayout->addWidget(loginInput);
    mainLayout->addWidget(new QLabel("Пароль:", this));
    mainLayout->addWidget(passwordInput);
    mainLayout->addWidget(messageLabel);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(registerButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
}

void LoginWindow::setupRegisterUI()
{
    currentState = Register;

    qDeleteAll(this->findChildren<QWidget*>());

    loginInput = new QLineEdit(this);
    loginInput->setPlaceholderText("Логин");

    emailInput = new QLineEdit(this);
    emailInput->setPlaceholderText("Email (необязательно)");

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Пароль");
    passwordInput->setEchoMode(QLineEdit::Password);

    confirmPasswordInput = new QLineEdit(this);
    confirmPasswordInput->setPlaceholderText("Подтвердите пароль");
    confirmPasswordInput->setEchoMode(QLineEdit::Password);

    registerAccountButton = new QPushButton("Зарегистрироваться", this);
    backButton = new QPushButton("Назад", this);

    messageLabel = new QLabel(this);
    messageLabel->setStyleSheet("color: red;");

    mainLayout->addWidget(new QLabel("Логин:", this));
    mainLayout->addWidget(loginInput);
    mainLayout->addWidget(new QLabel("Email:", this));
    mainLayout->addWidget(emailInput);
    mainLayout->addWidget(new QLabel("Пароль:", this));
    mainLayout->addWidget(passwordInput);
    mainLayout->addWidget(new QLabel("Подтвердите пароль:", this));
    mainLayout->addWidget(confirmPasswordInput);
    mainLayout->addWidget(messageLabel);
    mainLayout->addWidget(registerAccountButton);
    mainLayout->addWidget(backButton);

    connect(registerAccountButton, &QPushButton::clicked, this, &LoginWindow::onRegisterAccountClicked);
    connect(backButton, &QPushButton::clicked, this, &LoginWindow::onBackClicked);
}

void LoginWindow::setupLoggedInUI()
{
    currentState = LoggedIn;

    qDeleteAll(this->findChildren<QWidget*>());

    auto user = dbManager->findUserById(userId);
    QString name = user ? (user->nickname.isEmpty() ? user->login : user->nickname) : "Пользователь";
    welcomeLabel = new QLabel(QString("Добро пожаловать, %1!").arg(name), this);
    logoutButton = new QPushButton("Выйти из аккаунта", this);

    mainLayout->addWidget(welcomeLabel);
    mainLayout->addWidget(logoutButton);

    connect(logoutButton, &QPushButton::clicked, this, &LoginWindow::onLogoutClicked);
}

void LoginWindow::onLoginClicked()
{
    QString login = loginInput->text().trimmed();
    QString password = passwordInput->text();

    if (login.isEmpty() || password.isEmpty()) {
        messageLabel->setText("Логин и пароль обязательны!");
        return;
    }

    QString hash = QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());

    auto user = dbManager->findUserByLogin(login);
    if (user && user->password_hash == hash) {
        // Вход успешен
        userId = user->id;
        setupLoggedInUI();
    } else {
        messageLabel->setText("Неверный логин или пароль.");
    }
}

void LoginWindow::onRegisterClicked()
{
    setupRegisterUI();
}

void LoginWindow::onRegisterAccountClicked()
{
    QString login = loginInput->text().trimmed();
    QString email = emailInput->text().trimmed();
    QString password = passwordInput->text();
    QString confirm = confirmPasswordInput->text();

    if (login.isEmpty() || password.isEmpty()) {
        messageLabel->setText("Логин и пароль обязательны!");
        return;
    }

    if (password != confirm) {
        messageLabel->setText("Пароли не совпадают!");
        return;
    }

    QString hash = QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());

    if (dbManager->registerUser(login, login, email, hash)) {
        QMessageBox::information(this, "Успешно", "Регистрация прошла успешно!");
        setupLoginUI();
    } else {
        messageLabel->setText("Такой пользователь уже существует.");
    }
}

void LoginWindow::onBackClicked()
{
    setupLoginUI();
}

void LoginWindow::onLogoutClicked()
{
    userId = -1;
    setupLoginUI();
}

void LoginWindow::resetToLogin()
{
    userId = -1;
    setupLoginUI();
}
