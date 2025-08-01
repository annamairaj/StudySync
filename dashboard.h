#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QWidget>
#include <QVector>




namespace Ui {
class StudentDashboard;
}

struct Card {
    int id;
    QString question;
    QString answer;
};

class DashboardWindow : public QWidget
{
    Q_OBJECT
    friend class StudySyncTests;

public:
    explicit DashboardWindow(QWidget *parent = nullptr);
    ~DashboardWindow();

signals:
    void logoutSuccessful();

private slots:
    void dashboardButton_clicked();
    void subjectsButton_clicked();
    void certificatesButton_clicked();
    void dashboardLogoutButton_clicked();

    void addCardButton_clicked();

private:
    Ui::StudentDashboard *ui;
    //void addCardToList(const QString &question, const QString &answer);


    void addCardToList(const QString &question, const QString &answer, int cardId);
    int saveCardToDatabase(const QString &question, const QString &answer);
    void deleteCardFromDatabase(int cardId);
    QList<Card> loadAllCards();
};

#endif // DASHBOARDWINDOW_H

