#include "dashboard.h"
#include "ui_dashboard.h"

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::StudentDashboard)
{
    ui->setupUi(this);
    ui->progressBar->setValue(40);
    ui->downloadCertificateButton->setEnabled(false);
}

DashboardWindow::~DashboardWindow()
{
    delete ui;
}
