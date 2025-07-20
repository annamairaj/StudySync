#include "mainwindow.h"
#include "dashboard.h"
#include "welcomescreen.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    //WelcomeScreen w;
    w.show();
    return a.exec();

}
