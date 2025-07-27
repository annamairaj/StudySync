#ifndef PRODUCTIVITYPAGE_H
#define PRODUCTIVITYPAGE_H

#include <QWidget>

namespace Ui {
class ProductivityPage;
}

class ProductivityPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProductivityPage(QWidget *parent = nullptr);
    ~ProductivityPage();

    void refreshTotalTimePerSubject();
    void refreshSessionList(); // optional, for detailed sessions list

private slots:
    void on_addSesh_clicked();

private:
    Ui::ProductivityPage *ui;
    int currentUserId;  // store logged in user id here
};

#endif // PRODUCTIVITYPAGE_H
