#include "welcomescreen.h"
#include "ui_welcomescreen.h"
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QSettings>

bool WelcomeScreen::isValidEmail(const QString &email) {
    static const QRegularExpression regex(R"((^[^\s@]+@[^\s@]+\.[^\s@]+$))");
    return regex.match(email).hasMatch();
}

WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);

    ui->rememberMeCheckbox->setStyleSheet("QCheckBox { color: white; }"); //makes checkbox text visible

    // Start on home page (page 0)
    ui->stackedWidget->setCurrentIndex(0);

    // Connect to your named functions
    connect(ui->loginButton, &QPushButton::clicked, this, &WelcomeScreen::loginButton_clicked);
    connect(ui->signupButton, &QPushButton::clicked, this, &WelcomeScreen::signupButton_clicked);
    connect(ui->forgotButton, &QPushButton::clicked, this, &WelcomeScreen::forgotButton_clicked);
    connect(ui->forgotBackButton, &QPushButton::clicked, this, &WelcomeScreen::forgotBackButton_clicked);
    connect(ui->signupBackButton, &QPushButton::clicked, this, &WelcomeScreen::signupBackButton_clicked);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &WelcomeScreen::createAccountButton_clicked);

}


WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}

void WelcomeScreen::loginButton_clicked()
{

    QString identifier = ui->loginUsernameOrEmailField->text();
    QString email = ui->signupEmailField->text();
    QString password = ui->loginPasswordField->text();

    if (identifier.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login", "Please fill in all fields.");
        return;
    }

    // check to see if identifier looks like an email & validate it
    if (identifier.contains('@') && !isValidEmail(identifier)) {
        QMessageBox::warning(this, "Login", "Please enter a valid email address.");
        return;
    }

    if (validateLogin(identifier, password)) {
        //Remember me logic
        QSettings settings("Group 4", "StudySync");
        if (ui->rememberMeCheckbox->isChecked()){
            settings.setValue("remembered_user", identifier);
        } else {
            settings.remove("remembered_user");
        }
        //QMessageBox::information(this, "Login", "Login successful!");
        emit loginSuccessful();
    } else {
        QMessageBox::warning(this, "Login", "Invalid username/email or password.");
    }
}

void WelcomeScreen::createAccountButton_clicked()
{

    QString username = ui->signupUsernameField->text();
    QString email = ui->signupEmailField->text();
    QString password = ui->signupPasswordField->text();

        if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "Signup", "Please fill in all fields.");
            return;
        }

        // Validate email format
        if (!isValidEmail(email)) {
            QMessageBox::warning(this, "Signup", "Please enter a valid email address.");
            return;
        }

        //Matching pw check
        QString confirmPassword = ui->signupPasswordField->text();
        if (password != confirmPassword) {
            QMessageBox::warning(this, "Signup", "Passwords do not match.");
            return;
        }

        if (registerNewUser(username, email, password)) {
            ui->stackedWidget->setCurrentIndex(0); // back to login
            QMessageBox::information(this, "Signup", "Account created! Please login.");

        }
}

void WelcomeScreen::signupButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2); // Go to page 2
}

void WelcomeScreen::forgotButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1); // Back to home
}

void WelcomeScreen::forgotBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0); // Back to home
}

void WelcomeScreen::signupBackButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0); // Back to home
}

// ===== DATABASE =====
bool WelcomeScreen::registerNewUser(const QString &username, const QString &email, const QString &password)
{
    QString hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, email, password_hash) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(email);
    query.addBindValue(hash);

    if (query.exec()) {
        QMessageBox::information(this, "Signup", "User registered successfully!");
        return true;
    } else {
        QMessageBox::warning(this, "Signup", "Signup failed: " + query.lastError().text());
        qDebug() << "Signup error:" << query.lastError().text();
        return false;
    }
}


bool WelcomeScreen::validateLogin(const QString &identifier, const QString &password)
{
    QString hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();

    QSqlQuery query;
    query.prepare(R"(
        SELECT id FROM users
        WHERE (username = ? OR email = ?) AND password_hash = ?
    )");
    query.addBindValue(identifier);
    query.addBindValue(identifier);  // email and username are treated same
    query.addBindValue(hash);

    if (query.exec() && query.next()) {
        return true;
    } else {
        qDebug() << "Login failed:" << query.lastError().text();
        return false;
    }
}
