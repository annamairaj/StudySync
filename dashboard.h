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

private:
    Ui::StudentDashboard *ui;
};

#endif // DASHBOARDWINDOW_H
