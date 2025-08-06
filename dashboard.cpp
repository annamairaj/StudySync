#include "dashboard.h"
#include "ui_dashboard.h"
#include "addcarddialog.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QDate>
#include <QScrollArea>
#include <QScrollBar>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>


DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::StudentDashboard)
{
    ui->setupUi(this);

    QStringList subjects = {"Math", "Science", "History", "Literature"};
    ui->subjectSelector->addItems(subjects); //subject selector

    // This makes the the dashabord the first page when the user signs in
    ui->homeStackedWidget->setCurrentIndex(0);

    // This connects all the pages in the home screen
    connect(ui->dashboardButton, &QPushButton::clicked, this, &DashboardWindow::dashboardButton_clicked);
    connect(ui->subjectsButton, &QPushButton::clicked, this, &DashboardWindow::subjectsButton_clicked);
    connect(ui->certificatesButton, &QPushButton::clicked, this, &DashboardWindow::certificatesButton_clicked);
    connect(ui->dashboardLogoutButton, &QPushButton::clicked, this, &DashboardWindow::dashboardLogoutButton_clicked);
    connect(ui->updateProgressButton, &QPushButton::clicked, this, &DashboardWindow::updateProgress);
    connect(ui->subjectSelector, &QComboBox::currentTextChanged, this, &DashboardWindow::subjectSelector_changed);
    connect(ui->addCardButton, &QPushButton::clicked, this, &DashboardWindow::addCardButton_clicked);
    connect(ui->productivityTrackerButton, &QPushButton::clicked, this, &DashboardWindow::goToProductivityPage);

    if (!subjects.isEmpty()) {
        subjectSelector_changed(subjects.first());
    }


    // Load existing cards from DB
    QList<Card> cards = loadAllCards();
    for (const Card &card : cards) {
        addCardToList(card.question, card.answer, card.id);
    }

    // Populate notes & cert features
    updateCertificatesPage();
}


DashboardWindow::~DashboardWindow()
{
    delete ui;
}

void DashboardWindow::dashboardButton_clicked(){
    ui->homeStackedWidget->setCurrentIndex(0);

}

void DashboardWindow::goToProductivityPage() {
    ui->homeStackedWidget->setCurrentIndex(1);
}

void DashboardWindow::subjectsButton_clicked() {
    ui->homeStackedWidget->setCurrentIndex(2);
    updateNotesSection();
}

void DashboardWindow::certificatesButton_clicked(){
    ui->homeStackedWidget->setCurrentIndex(3);

}

void DashboardWindow::dashboardLogoutButton_clicked(){
    emit logoutSuccessful();       // Notify mainwindow.cpp
}

// Display most recent progress
void DashboardWindow::subjectSelector_changed(const QString &subject)
{
    qDebug() << "Subject changed to:" << subject;

    int userId = 1;

    QSqlQuery query;
    query.prepare(R"(
        SELECT progress, notes, date FROM progress
        WHERE user_id = ? AND subject = ?
        ORDER BY date DESC
        LIMIT 1
    )");
    query.addBindValue(userId);
    query.addBindValue(subject);

    if (query.exec() && query.next()) {
        int progressValue = query.value(0).toInt();
        QString notes = query.value(1).toString();
        QString dateStr = query.value(2).toString();
        QDate date = QDate::fromString(dateStr, Qt::ISODate);

        ui->prodProgressBar->setValue(progressValue);
        ui->notesEdit->setText(notes);
        ui->datePicker->setDate(date);

        qDebug() << "Loaded progress for" << subject << ":" << progressValue;
    } else {
        // No progress saved yet for this subject — reset UI
        ui->prodProgressBar->setValue(0);
        ui->notesEdit->clear();
        ui->datePicker->setDate(QDate::currentDate());

        qDebug() << "No progress found for" << subject;
    }
}

// Increase progress and trigger certificate creation
void DashboardWindow::updateProgress()
{
    QString subject = ui->subjectSelector->currentText();
    QString notes = ui->notesEdit->text();
    QDate date = ui->datePicker->date();


    int oldValue = ui->prodProgressBar->value();
    int newValue = qMin(oldValue + 10, 100); //increases percentage by 10 each time

    // Save to DB
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO progress (user_id, subject, progress, notes, date)
        VALUES (?, ?, ?, ?, ?)
    )");

    query.addBindValue(1);
    query.addBindValue(subject);
    query.addBindValue(newValue);
    query.addBindValue(notes);
    query.addBindValue(date.toString(Qt::ISODate));

    if (!query.exec()) {
        qDebug() << "Failed to save progress:" << query.lastError();
        ui->productivityPageLabel->setText("Failed to save progress.");
    } else {
        ui->productivityPageLabel->setText(QString("Progress saved for %1").arg(subject));

        // Animate from oldValue to newValue
        QPropertyAnimation *animation = new QPropertyAnimation(ui->prodProgressBar, "value");
        animation->setDuration(600);
        animation->setStartValue(oldValue);
        animation->setEndValue(newValue);
        animation->start(QAbstractAnimation::DeleteWhenStopped);

        // Actually update the value (so it's stored for future logic)
        ui->prodProgressBar->setValue(newValue);


        if (newValue == 100 && oldValue < 100) {
            // Show popup congratulation message
            QMessageBox msgBox;
            msgBox.setWindowTitle("🎉 You Did It!");
            msgBox.setText("⭐ Congratulations! ⭐ \n\nYou've mastered this subject!\n ✨Keep up the amazing work✨");
            msgBox.exec();

            //Generate certificate if just reached 100%
            QSqlQuery certQuery;
            certQuery.prepare("INSERT INTO certificates (user_id, subject, date_generated) VALUES (?, ?, ?)");
            certQuery.addBindValue(1);
            certQuery.addBindValue(subject);
            certQuery.addBindValue(QDate::currentDate().toString(Qt::ISODate));
            certQuery.exec();
            updateCertificatesPage();
        }
        updateNotesSection();
    }
}

// Loads saved notes grouped by subject (sorted by date)
void DashboardWindow::updateNotesSection() {
    ui->pastStudySessionNotes->clear();

    QSqlQuery query;
    query.prepare("SELECT subject, date, notes FROM progress WHERE user_id = 1 ORDER BY subject, date DESC");
    if (!query.exec()) {
        qDebug() << "Failed to load notes:" << query.lastError();
        return;
    }

    while (query.next()) {
        QString subject = query.value(0).toString();
        QString date = query.value(1).toString();
        QString notes = query.value(2).toString();
        QString displayText = QString("[%1] %2\n%3").arg(subject, date, notes);
        ui->pastStudySessionNotes->addItem(displayText);
    }
}

// Displays certificate list
void DashboardWindow::updateCertificatesPage() {
    ui->certListWidget->clear();

    QSqlQuery query("SELECT subject, date_generated FROM certificates WHERE user_id = 1");
    while (query.next()) {
        QString subject = query.value(0).toString();
        QString date = query.value(1).toString();
        QString itemText = QString("📜 %1 — %2").arg(subject, date);
        ui->certListWidget->addItem(itemText);
    }
}

void DashboardWindow::addCardButton_clicked()
{
    AddCardDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString question = dialog.getQuestion();
        QString answer = dialog.getAnswer();

        int id = saveCardToDatabase(question, answer);
        if (id != -1)
            addCardToList(question, answer, id);
    }
}

void DashboardWindow::addCardToList(const QString &question, const QString &answer, int cardId)
{
    QWidget *cardWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(cardWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton *qaButton = new QPushButton(question);
    qaButton->setCheckable(true);

        qaButton->setStyleSheet(R"(
        QPushButton {
            background-color: white;
            color: black;
            height: 50px;
            font-size: 16px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #f0f0f0;
        }
        QPushButton:checked {
            background-color: #e0e0e0;
        }
    )");

    connect(qaButton, &QPushButton::toggled, this, [=](bool checked) {
        qaButton->setText(checked ? answer : question);
    });

    QPushButton *deleteButton = new QPushButton("Delete");
    // deleteButton->setStyleSheet("QPushButton { color: red; }");
    deleteButton->setStyleSheet(R"(
        QPushButton {
            background-color: rgb(238, 104, 115);
            color: white;
            font-weight: bold;
            height: 50px;
            padding: 0 20px;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #a80000;
        }
    )");


    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        deleteCardFromDatabase(cardId);
        ui->cardLayout->removeWidget(cardWidget);
        cardWidget->deleteLater();
    });

    layout->addWidget(qaButton, 4);
    layout->addWidget(deleteButton, 1);
    ui->cardLayout->addWidget(cardWidget);
}


int DashboardWindow::saveCardToDatabase(const QString &question, const QString &answer)
{
    QSqlQuery query;
    query.prepare("INSERT INTO cards (user_id, question, answer) VALUES (?, ?, ?)");
    query.addBindValue(1); // replace with actual user_id
    query.addBindValue(question);
    query.addBindValue(answer);
    if (query.exec()) {
        return query.lastInsertId().toInt();
    } else {
        qDebug() << "Insert error:" << query.lastError();
        return -1;
    }
}

void DashboardWindow::deleteCardFromDatabase(int cardId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM cards WHERE id = ?");
    query.addBindValue(cardId);
    if (!query.exec()) {
        qDebug() << "Delete error:" << query.lastError();
    }
}

QList<Card> DashboardWindow::loadAllCards()
{
    QList<Card> cardList;
    QSqlQuery query;
    query.prepare("SELECT id, question, answer FROM cards WHERE user_id = ?");
    query.addBindValue(1); // replace with actual user_id
    if (query.exec()) {
        while (query.next()) {
            Card card;
            card.id = query.value(0).toInt();
            card.question = query.value(1).toString();
            card.answer = query.value(2).toString();
            cardList.append(card);
        }
    } else {
        qDebug() << "Load error:" << query.lastError();
    }
    return cardList;
}



