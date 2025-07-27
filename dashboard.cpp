#include "dashboard.h"
#include "ui_dashboard.h"
#include "databasemanager.h"
#include <QMessageBox>
#include <QDate>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::StudentDashboard)
{
    ui->setupUi(this);

    // Set dashboard page as default
    ui->homeStackedWidget->setCurrentIndex(0);

    // Connect page navigation buttons
    connect(ui->dashboardButton, &QPushButton::clicked, this, &DashboardWindow::dashboardButton_clicked);
    connect(ui->subjectsButton, &QPushButton::clicked, this, &DashboardWindow::subjectsButton_clicked);
    connect(ui->certificatesButton, &QPushButton::clicked, this, &DashboardWindow::certificatesButton_clicked);
    connect(ui->dashboardLogoutButton, &QPushButton::clicked, this, &DashboardWindow::dashboardLogoutButton_clicked);
    connect(ui->productivityButton, &QPushButton::clicked, this, &DashboardWindow::productivityButton_clicked);
    connect(ui->addSesh, &QPushButton::clicked, this, &DashboardWindow::addSession);

    // Populate subjects including "Add Subject" for custom subject
    ui->subjectPick->addItems({
        "Math",
        "Science",
        "History",
        "English",
        "Computer Science",
        "Art",
        "Add Subject"
    });

    // Hide line edit and add button by default
    ui->customSubject->hide();
    ui->addSubButton->hide();

    // Show/hide input when selecting "Add Subject"
    connect(ui->subjectPick, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index)
            {
                QString selected = ui->subjectPick->itemText(index);
                if (selected == "Add Subject") {
                    ui->customSubject->show();
                    ui->addSubButton->show();
                } else {
                    ui->customSubject->hide();
                    ui->addSubButton->hide();
                }
            });

    // Add new subject when button clicked
    connect(ui->addSubButton, &QPushButton::clicked, this, [=]()
            {
                QString newSubject = ui->customSubject->text().trimmed();
                if (!newSubject.isEmpty() && ui->subjectPick->findText(newSubject) == -1) {
                    int insertIndex = ui->subjectPick->count() - 1; // before "Add Subject"
                    ui->subjectPick->insertItem(insertIndex, newSubject);
                    ui->subjectPick->setCurrentIndex(insertIndex); // switch to new subject
                    ui->customSubject->clear();

                    // Show confirmation message
                    QMessageBox::information(this, "Subject Added", "Subject \"" + newSubject + "\" has been added.");
                } else if (ui->subjectPick->findText(newSubject) != -1) {
                    QMessageBox::warning(this, "Duplicate Subject", "This subject already exists.");
                }

                ui->customSubject->hide();
                ui->addSubButton->hide();
            });
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

void DashboardWindow::productivityButton_clicked() {
    ui->homeStackedWidget->setCurrentIndex(3);
}

void DashboardWindow::certificatesButton_clicked(){
    ui->homeStackedWidget->setCurrentIndex(2);
}

//function to make addSession button work
void DashboardWindow::addSession() {
    // Get data from UI
    QString date = ui->dateEdit->date().toString("yyyy-MM-dd"); // Format date as string
    QString subject = ui->subjectPick->currentText().trimmed();
    int duration = ui->timeSpent->value();
    QString notes = ui->noteEdit->text().trimmed();

    // Basic validation
    if (subject.isEmpty() || subject == "Add Subject") {
        QMessageBox::warning(this, "Invalid Input", "Please select a valid subject.");
        return;
    }
    if (duration <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a duration greater than 0.");
        return;
    }

    int userId = 1; // Replace with actual logged-in user ID as needed

    bool success = dbManager.addStudySession(userId, date, subject, duration, notes);
    if (success) {
        QMessageBox::information(this, "Session Added", "Your study session has been recorded.");
        // Clear inputs
        ui->timeSpent->setValue(0);
        ui->noteEdit->clear();
        ui->subjectPick->setCurrentIndex(0);
        ui->dateEdit->setDate(QDate::currentDate());
    } else {
        QMessageBox::critical(this, "Error", "Failed to save this study session.");
    }
}

void DashboardWindow::dashboardLogoutButton_clicked(){
    emit logoutSuccessful(); // notify main window
}

