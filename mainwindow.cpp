#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dashboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    // Makes the UI custom size and color
    resize(1000, 900);
    setStyleSheet("QMainWindow { border: none; background-color: rgb(17, 25, 34); }");

    // Create the stack to hold main pages
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);  // Set this as central layout of main window

    // Initialize major screens
    welcomeScreen = new WelcomeScreen(this);
    dashboard = new DashboardWindow(this);

    // Add screens to the stack
    mainStack->addWidget(welcomeScreen);      // Index 0
    mainStack->addWidget(dashboard);    // Index 1

    // Show welcome screen by default
    mainStack->setCurrentWidget(welcomeScreen);

    // Connect login signal to switch screen
    connect(welcomeScreen, &WelcomeScreen::loginSuccessful, this, [=]() {
        mainStack->setCurrentWidget(dashboard);
    });

    connect(dashboard, &DashboardWindow::logoutSuccessful, this, [=]() {
        mainStack->setCurrentWidget(welcomeScreen);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}
