#include "mainwindow.h"
#include "dashboard.h"
#include "welcomescreen.h"
#include <QApplication>
#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::initialize()) {
        return 1;
    }

    MainWindow w;
    w.show();
    return a.exec();

}
