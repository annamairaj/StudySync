#include "dashboard.h"
#include "ui_dashboard.h"

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::StudentDashboard)
{
    ui->setupUi(this);

    //void dashboardButton_clicked();
    //void subjectsButton_clicked();
    //void certificatesButton_clicked();

    // This makes the the dashabord the first page when the user signs in
    ui->homeStackedWidget->setCurrentIndex(0);

    // This connects all the pages in the home screen
    connect(ui->dashboardButton, &QPushButton::clicked, this, &DashboardWindow::dashboardButton_clicked);
    connect(ui->subjectsButton, &QPushButton::clicked, this, &DashboardWindow::subjectsButton_clicked);
    connect(ui->certificatesButton, &QPushButton::clicked, this, &DashboardWindow::certificatesButton_clicked);
    connect(ui->dashboardLogoutButton, &QPushButton::clicked, this, &DashboardWindow::dashboardLogoutButton_clicked);

}

DashboardWindow::~DashboardWindow()
{
    delete ui;
}

void DashboardWindow::dashboardButton_clicked(){
    ui->homeStackedWidget->setCurrentIndex(0);

}

void DashboardWindow::subjectsButton_clicked(){
    ui->homeStackedWidget->setCurrentIndex(1);

}

void DashboardWindow::certificatesButton_clicked(){
    ui->homeStackedWidget->setCurrentIndex(2);

}

void DashboardWindow::dashboardLogoutButton_clicked(){
    emit logoutSuccessful();       // Notify mainwindow.cpp
}

