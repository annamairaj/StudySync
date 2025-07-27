#include "productivityPage.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

ProductivityPage::ProductivityPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProductivityPage)
{
    ui->setupUi(this);
    currentUserId = /* get logged in user ID somehow */;

    // Connect button clicked signal to slot
    connect(ui->addSesh, &QPushButton::clicked, this, &ProductivityPage::on_addSesh_clicked);

    // Initialize UI elements as needed
    refreshTotalTimePerSubject();
}

ProductivityPage::~ProductivityPage()
{
    delete ui;
}

void ProductivityPage::on_addSesh_clicked() {
    // (Use the add session logic from previous message here)
}

void ProductivityPage::refreshTotalTimePerSubject() {
    // (Use the refresh total time logic from previous message here)
}


