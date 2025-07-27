
#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H
#include "databasemanager.h"
#include <QWidget>

namespace Ui {
class StudentDashboard;
}

class DashboardWindow : public QWidget
{
    Q_OBJECT

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
    void productivityButton_clicked();
    void addSession();

private:
    Ui::StudentDashboard *ui;
    DatabaseManager dbManager;
};

#endif // DASHBOARDWINDOW_H
