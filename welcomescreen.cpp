#include "welcomescreen.h"
#include "ui_welcomescreen.h"

WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);

    // Start on home page (page 0)
    ui->stackedWidget->setCurrentIndex(0);

    // Connect to your named functions
    connect(ui->loginButton, &QPushButton::clicked, this, &WelcomeScreen::loginButton_clicked);
    connect(ui->signupButton, &QPushButton::clicked, this, &WelcomeScreen::signupButton_clicked);
    connect(ui->forgotButton, &QPushButton::clicked, this, &WelcomeScreen::forgotButton_clicked);
    connect(ui->forgotBackButton, &QPushButton::clicked, this, &WelcomeScreen::forgotBackButton_clicked);
    connect(ui->signupBackButton, &QPushButton::clicked, this, &WelcomeScreen::signupBackButton_clicked);
}


WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}

void WelcomeScreen::loginButton_clicked()
{
    emit loginSuccessful();
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
