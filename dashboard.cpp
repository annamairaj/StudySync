// #include "dashboard.h"
// #include "ui_dashboard.h"

// #include "addcarddialog.h"
// #include <QScrollArea>
// #include <QVBoxLayout>
// #include <QPushButton>
// #include <QLabel>


#include "dashboard.h"
#include "ui_dashboard.h"
#include "addcarddialog.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>

DashboardWindow::DashboardWindow(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::StudentDashboard)
{
    ui->setupUi(this);


    // This makes the the dashabord the first page when the user signs in
    ui->homeStackedWidget->setCurrentIndex(0);

    // This connects all the pages in the home screen
    connect(ui->dashboardButton, &QPushButton::clicked, this, &DashboardWindow::dashboardButton_clicked);
    connect(ui->subjectsButton, &QPushButton::clicked, this, &DashboardWindow::subjectsButton_clicked);
    connect(ui->certificatesButton, &QPushButton::clicked, this, &DashboardWindow::certificatesButton_clicked);
    connect(ui->dashboardLogoutButton, &QPushButton::clicked, this, &DashboardWindow::dashboardLogoutButton_clicked);

    connect(ui->addCardButton, &QPushButton::clicked, this, &DashboardWindow::addCardButton_clicked);

    // Load existing cards from DB
    QList<Card> cards = loadAllCards();
    for (const Card &card : cards) {
        addCardToList(card.question, card.answer, card.id);
    }

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

// void DashboardWindow::addCardButton_clicked()
// {
//     AddCardDialog dialog(this);
//     if (dialog.exec() == QDialog::Accepted) {
//         QString question = dialog.getQuestion();
//         QString answer = dialog.getAnswer();
//         addCardToList(question, answer);
//     }
// }

// void DashboardWindow::addCardToList(const QString &question, const QString &answer)
// {
//     QPushButton *cardButton = new QPushButton(question);
//     cardButton->setCheckable(true);  // allows toggle

//     connect(cardButton, &QPushButton::toggled, this, [=](bool checked) {
//         cardButton->setText(checked ? answer : question);
//     });

//     ui->cardLayout->addWidget(cardButton);
// }


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
    connect(qaButton, &QPushButton::toggled, this, [=](bool checked) {
        qaButton->setText(checked ? answer : question);
    });

    QPushButton *deleteButton = new QPushButton("Delete");
    deleteButton->setStyleSheet("QPushButton { color: red; }");
    connect(deleteButton, &QPushButton::clicked, this, [=]() {
        deleteCardFromDatabase(cardId);
        ui->cardLayout->removeWidget(cardWidget);
        cardWidget->deleteLater();
    });

    layout->addWidget(qaButton);
    layout->addWidget(deleteButton);
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



