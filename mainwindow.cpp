#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dashboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::handleLoginClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::handleLoginClicked()
{
    this->hide(); //hide login window
    DashboardWindow *dashboard = new DashboardWindow();
    dashboard->show();
}
