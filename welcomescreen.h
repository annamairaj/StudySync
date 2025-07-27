
#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QWidget>

namespace Ui {
class WelcomeScreen;
}

class WelcomeScreen : public QWidget
{
    Q_OBJECT

public:
    explicit WelcomeScreen(QWidget *parent = nullptr);
    ~WelcomeScreen();

signals:
    void loginSuccessful();

private slots:
    void loginButton_clicked();
    void signupButton_clicked();
    void forgotButton_clicked();
    void forgotBackButton_clicked();
    void signupBackButton_clicked();
    void createAccountButton_clicked();

private:
    bool registerNewUser(const QString &username, const QString &email, const QString &password);
    bool validateLogin(const QString &identifier, const QString &password);
    bool isValidEmail (const QString &email);


private:
    Ui::WelcomeScreen *ui;
};

#endif // WELCOMESCREEN_H
