#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

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

private:
    Ui::StudentDashboard *ui;
};

#endif // DASHBOARDWINDOW_H

